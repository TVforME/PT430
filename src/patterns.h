/*
 * Copyright (C) 2024  Robert Hensel VK3DG <vk3dgtv@gmail.com>
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

 pattern.h - include for patterns.c
 
 */

#ifndef PATTERNS_H
#define PATTERNS_H

#include "debug.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>  

// Constants

// EPROM parameters
#define EPROM_SIZE       8192    // 27C64 is an 8K device

// Pattern boundaries
#define LINE_WIDTH       128
#define PIXELS_PER_LINE  128     // A0-A6 (0-127)
#define LINES_PER_FIELD  140     // Lines for the main pattern
#define TEXT_LINES       14      // A7-A10 for text area
#define PATTERN_SIZE     0x800   // Size per pattern (2KB)

// Pattern memory locations (base addresses)
#define PATTERN_BARS     0x0000  // A11=0, A12=0 (Color Bars)
#define PATTERN_RED		 0x0800  // A11=1, A12=0 (Spilt Field Red)
#define PATTERN_PULSE    0x1000  // A11=0, A12=1 (Pulse & Bar)
#define PATTERN_BLACK    0x1800  // A11=1, A12=1 (Color black)

// Pattern offsets
#define TEXT_START        0x080   // Start of text overlay area
#define TEXT_AREA_OFFSET  0x800   // Text area in each pattern block
#define LINE_16_OFFSET    0x780   // Start of line 16 pattern

// Character and text bitmap dimensions
#define CHAR_WIDTH                          5   // Width of each character in pixels
#define TEXT_BITMAP_HEIGHT                  7   // Height of the text bitmap
#define TEXT_INTER_SPACE                    2   // 2 pixel inter space between chars
#define TEXT_BITMAP_WIDTH PIXELS_PER_LINE       // Width of the text bitmap
#define TEXT_KEEPOUT                       16   // Keep a BAR WIDTH each side to keep Text visable between WHITE (left) BLACK (right)
#define MAX_TEXT_LENGTH                    14   // No more than 14 chars of text


// Color bar widths
#define BAR_WIDTH       16      // Each color bar is 16 pixels wide
#define NUM_BARS        8       // Number of color bars
#define PULSE_OFFSET    8       // Offset for pulse in magenta bar
#define BAR_POSITION    6       // Position of white bar (in blue position)
#define MAGENTA_BAR     4       // Position of magenta bar for pulse

// Color values
#define COLOR_WHITE     0xFF
#define COLOR_YELLOW    0xF3
#define COLOR_CYAN      0xF5
#define COLOR_GREEN     0xF1
#define COLOR_MAGENTA   0xF6
#define COLOR_RED       0xF2
#define COLOR_BLUE      0xF4
#define COLOR_BLACK     0xF0

// Video output bit positions (GRBW)
#define GREEN_BIT 0
#define RED_BIT   1
#define BLUE_BIT  2
#define WHITE_BIT 3


// text char bitmaps, 5x7 font mapping. 
static const unsigned char font_data[] = {
    	0x00, 0x00, 0x00, 0x00, 0x00, // (space)
    	0x7E, 0x11, 0x11, 0x11, 0x7E, // A
	0x7F, 0x49, 0x49, 0x49, 0x36, // B
	0x3E, 0x41, 0x41, 0x41, 0x22, // C
	0x7F, 0x41, 0x41, 0x22, 0x1C, // D
	0x7F, 0x49, 0x49, 0x49, 0x41, // E
	0x7F, 0x09, 0x09, 0x01, 0x01, // F
	0x3E, 0x41, 0x41, 0x51, 0x32, // G
	0x7F, 0x08, 0x08, 0x08, 0x7F, // H
	0x00, 0x41, 0x7F, 0x41, 0x00, // I
	0x20, 0x40, 0x41, 0x3F, 0x01, // J
	0x7F, 0x08, 0x14, 0x22, 0x41, // K
	0x7F, 0x40, 0x40, 0x40, 0x40, // L
	0x7F, 0x02, 0x04, 0x02, 0x7F, // M
	0x7F, 0x04, 0x08, 0x10, 0x7F, // N
	0x3E, 0x41, 0x41, 0x41, 0x3E, // O
	0x7F, 0x09, 0x09, 0x09, 0x06, // P
	0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
	0x7F, 0x09, 0x19, 0x29, 0x46, // R
	0x26, 0x49, 0x49, 0x49, 0x32, // S
	0x01, 0x01, 0x7F, 0x01, 0x01, // T
	0x3F, 0x40, 0x40, 0x40, 0x3F, // U
	0x1F, 0x20, 0x40, 0x20, 0x1F, // V
	0x7F, 0x20, 0x18, 0x20, 0x7F, // W
	0x63, 0x14, 0x08, 0x14, 0x63, // X
	0x03, 0x04, 0x78, 0x04, 0x03, // Y
	0x61, 0x51, 0x49, 0x45, 0x43, // X
	0x3E, 0x51, 0x49, 0x45, 0x3E, // Z
	0x00, 0x42, 0x7F, 0x40, 0x00, // 1
	0x42, 0x61, 0x51, 0x49, 0x46, // 2
	0x21, 0x41, 0x45, 0x4B, 0x31, // 3
	0x18, 0x14, 0x12, 0x7F, 0x10, // 4
	0x27, 0x45, 0x45, 0x45, 0x39, // 5
	0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
	0x01, 0x71, 0x09, 0x05, 0x03, // 7
	0x36, 0x49, 0x49, 0x49, 0x36, // 8
	0x06, 0x49, 0x49, 0x29, 0x1E, // 9
    	0x08, 0x08, 0x08, 0x08, 0x08, // -
	0x00, 0x36, 0x36, 0x00, 0x00, // :
};


// Pattern generation functions
uint8_t generateColorBar(int pixel_pos);
uint8_t generatePulseBar(int pixel_pos);
void generateTextBitmap(const char* text, uint8_t* bitmap);
bool generateEpromData(uint8_t* eprom_data, uint8_t* bitmap_data, const char* id_text);
void sanitizeFilename(char* filename);

#endif // PATTERNS_H

