/*
 * Copyright (C) 2025  Robert Hensel VK3DG <vk3dgtv@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.

 patterns.c  All the functions which do all the heavy work generating
             the color bar, split field, pulse & bar and color black patterns.
             generation of TEXT ID and overlay into eprom buffer.

// PRACTEL PT-430b 27C64-150 buffer dump
// Size: 8192 bytes (0x2000)
// Format: Raw hex dump, 16 bytes per line
//
// Address Pattern Layout:
//
// 0x0000-0x07FF: Pattern 1 - Color Bars (A11=0, A12=0) Select lines
//   - 0x0000-0x007F: Initial pattern. Lines 1 - 140 (repeats 128 pixels)
//   - 0x0080-0x077F: Main pattern with text overlay. Lines 141 - 154 alternates odd/even fields.         
//   - 0x0780-0x07FF: Line 16 pattern. color bars. Lines 156 - 310?  Repeats till vertical sync reset.
//
// 0x0800-0x0FFF: Pattern 2 - Split Field Red (A11=1, A12=0) Select lines
//   - 0x0800-0x087F: Initial pattern. Lines 1 - 140 (repeats 128 pixels)
//   - 0x0880-0x0F7F: Main pattern with text overlay. Lines 141 - 154 alternates odd/even fields. 
//   - 0x0F80-0x0FFF: Line 16 pattern color red. Lines 156 - 310?  Repeats till vertical sync reset.
//
// 0x1000-0x17FF: Pattern 3 - Pulse & Bar (A11=0, A12=1) Select lines
//   - 0x1000-0x107F: Initial pattern. Lines 1 - 140 (repeats 128 pixels)
//   - 0x1080-0x177F: Main pattern with text overlay. Lines 141 - 154 alternates odd/even fields.
//   - 0x1780-0x17FF: Line 16 with pulse and bar. Lines 156 - 310?  Repeats till vertical sync reset.
//
// 0x1800-0x1FFF: Pattern 4 - Unused Color Black (A11=1, A12=1) Select lines
//   - 0x1800-0x187F: Initial pattern. Lines 1 - 140 (repeats 128 pixels)
//   - 0x1880-0x1F7F: Main pattern area. Lines 141 - 154 alternates odd/even fields.
//   - 0x1F80-0x1FFF: Line 16 pattern. color black. Lines 156 - 310?  Repeats till vertical sync reset.          
 */

#include "patterns.h"
#include "output.h"
#include <string.h>
#include <ctype.h>

// Helper function to get character index
static int getCharIndex(char c) {
    c = toupper(c);
    if (c >= 'A' && c <= 'Z') return (c - 'A' + 1);  // A-Z at indices 1-26
    if (c >= '0' && c <= '9') return (c - '0' + 27); // 0-9 at indices 27-36
    if (c == '-') return 37;                         // Hyphen at index 37
    if (c == ':') return 38;                         // Colon at index 38
    return -1; // Return -1 for unknown characters.
}

// Color bar pattern generator
// All weirdness the last bar black is first
uint8_t generateColorBar(int pixel_pos) {
    int bar = (pixel_pos / BAR_WIDTH) % NUM_BARS;

    switch (bar) {
        case 0: return COLOR_BLACK;
        case 1: return COLOR_WHITE;
        case 2: return COLOR_YELLOW;
        case 3: return COLOR_CYAN;
        case 4: return COLOR_GREEN;
        case 5: return COLOR_MAGENTA;
        case 6: return COLOR_RED;
        case 7: return COLOR_BLUE;
        default: return COLOR_BLACK;
    }
}

// Pulse and Bar pattern generator
uint8_t generatePulseBar(int pixel_pos) {
    // Calculate positions relative to color bars
    int magenta_center = (MAGENTA_BAR * BAR_WIDTH) + PULSE_OFFSET;
    int bar_start = BAR_POSITION * BAR_WIDTH;

    if (pixel_pos == magenta_center) {
        return COLOR_WHITE;  // Single pixel pulse
    } else if (pixel_pos >= bar_start && pixel_pos < bar_start + BAR_WIDTH) {
        return COLOR_WHITE;  // Bar
    } else {
        return COLOR_BLACK;
    }
}

// Function to generate a text bitmap chars
void generateTextBitmap(const char* text, uint8_t* bitmap) {
    int text_length = strlen(text);
    if (text_length > 14) {
        text_length = 14; // Truncate text if too long
        fprintf(stderr, "Warning: Text truncated to 14 characters.\n");
    }

    int char_width_with_space = CHAR_WIDTH + TEXT_INTER_SPACE; // Character width + inter-character spacing
    int text_width = text_length * char_width_with_space - TEXT_INTER_SPACE; // Calculate total width (without trailing space)

    // Center the text with keep-out areas
    int available_width = PIXELS_PER_LINE - (2 * TEXT_KEEPOUT);
    int text_start = TEXT_KEEPOUT + (available_width - text_width) / 2 + 1;

    // Create a blank bitmap. 128 x 7 = 896 positions
    memset(bitmap, 0, PIXELS_PER_LINE * TEXT_BITMAP_HEIGHT);

    int current_x = text_start;
    int reduced_width_count = 0;

    for (int line = 0; line < TEXT_BITMAP_HEIGHT; line++) {
        current_x = text_start;
        reduced_width_count = 0;

        for (int char_pos = 0; char_pos < text_length; char_pos++) {
            char c = text[char_pos];

            // Handle space character FIRST
            if (c == ' ') {
                current_x += CHAR_WIDTH + TEXT_INTER_SPACE; // Advance current_x for space
                continue; // Skip to the next character
            }
            // return index of current char from bitmap array.
            int char_index = getCharIndex(c);

            // Begin formating other chars except space.
            if (char_index != -1) {
                int font_offset = char_index * CHAR_WIDTH;
                int char_width = CHAR_WIDTH;

                // special condition to reduce 'I' and '1' be a pixel each side. (match original text mapping)
                if (c == '1' || c == 'I') {
                    char_width--; // Reduce the width for '1' and 'I' to by a pixel left.
                    current_x -= (reduced_width_count + 1); // Shift '1'/'I' and account for previous reduced width chars by a pixel right.
                } else {
                    // Adjust current_x for reduced-width characters encountered so far (for other chars)
                    current_x -= reduced_width_count;
                }

                for (int char_pixel = 0; char_pixel < CHAR_WIDTH; char_pixel++) {
                    if (font_data[font_offset + char_pixel] & (1 << line)) {
                        int pixel_pos = current_x + char_pixel;
                        if (pixel_pos < PIXELS_PER_LINE) {
                            bitmap[line * PIXELS_PER_LINE + pixel_pos] = COLOR_WHITE;
                        }
                    }
                }
                current_x += char_width + TEXT_INTER_SPACE;  
            }
        }
    }
}

// Generate pattern format in EPROM buffer.
bool generateEpromData(uint8_t* eprom_data, uint8_t* bitmap_data, const char* id_text) {
    if (!eprom_data) {
        fprintf(stderr, "Error: NULL EPROM data buffer\n");
        return false;
    }

   // Initialize EPROM buffer to black.
    memset(eprom_data, COLOR_BLACK, EPROM_SIZE);

    // Generate the text bitmap
    generateTextBitmap(id_text, bitmap_data);
    
    //  2K jump to the pattern offset.
    for (int section = 0; section < 4; section++) {
        int base_addr;
        switch(section) {
            case 0: base_addr = PATTERN_BARS; break;
            case 1: base_addr = PATTERN_RED; break;
            case 2: base_addr = PATTERN_PULSE; break;
            case 3: base_addr = PATTERN_BLACK; break;
        }

        // Create the top 139 lines of pattern which is always color bars
        // 74HC383B continually count 128 pixels.
        for (int pixel = 0; pixel < PIXELS_PER_LINE; pixel++) {
            int addr = base_addr + pixel;
            uint8_t pattern;

            if (section == 0 || section == 1 || section == 2) {
                pattern = generateColorBar(pixel);
            } else {
                pattern = COLOR_BLACK;
                continue; 
            }

            eprom_data[addr] = pattern;
        }

        // loop through each 14 lines to apply the base pattern.
        for (int line = 0; line < 7; line++) { // 7 lines  with same text on EVEN & ODD fields.

            // Even/Odd fields flipper.
            for (int field = 0; field < 2; field++) {
                int line_addr = base_addr + TEXT_START + (line * 256) + (field * 128);
                // Pattern background write to buffer
                for (int pixel = 0; pixel < PIXELS_PER_LINE; pixel++) {
                    int addr = line_addr + pixel;
                    uint8_t pattern;
                    // Lay pattern behind TEXT overlay for colorbars and pulse&bar.
                    if (section == 0 || section == 1 || section == 2) {
                        pattern = generateColorBar(pixel);
                    } else {
                        pattern = COLOR_BLACK;
                        continue; 
                    }
                    // Apply text overlay line from char bitmap over pattern
                    if ((section == 0 || section == 1 || section == 2) && bitmap_data[line * PIXELS_PER_LINE + pixel]) {
                        pattern = COLOR_WHITE;
                    }
                    eprom_data[addr] = pattern;
                }
            }
        }

        // line 16 pattern to end of field.
        for (int pixel = 0; pixel < PIXELS_PER_LINE; pixel++) {
            int addr = base_addr + LINE_16_OFFSET + pixel;
            uint8_t pattern;

            if (section == 0) {
                pattern = generateColorBar(pixel);
            } else if (section == 1) {
                pattern = COLOR_RED;
            } else if (section == 2) {
                pattern = generatePulseBar(pixel);
            } else {
                pattern = COLOR_BLACK;
                continue; 
            }

            eprom_data[addr] = pattern;
        }

    }

    return true;
}
