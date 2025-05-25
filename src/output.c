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

 output.c  handles writing eprom.hex, eprom.bin and helper files
 */


#include "output.h"
#include "patterns.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

// Write binary file
int writeBinFile(const uint8_t* data, int length, const char* filename) {
    printf("Writing binary file: %s\n", filename);
    
    FILE* fp = fopen(filename, "wb");  // Note: binary mode
    if(!fp) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
        return 0;
    }
    
    size_t written = fwrite(data, 1, length, fp);
    fclose(fp);
    
    if (written != length) {
        fprintf(stderr, "Error: Only wrote %zu of %d bytes\n", written, length);
        return 0;
    }
    
    printf("Binary file written successfully:\n");
    printf("  - File: %s\n", filename);
    printf("  - Size: %d bytes\n", length);
    
    return 1;
}


// HEX file writer
int writeHexFile(const uint8_t* data, int length, const char* filename) {
    printf("Writing INTEL HEX file: %s\n", filename);
    FILE* fp = fopen(filename, "w");
    if(!fp) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
        return 0;
    }
    
    int addr = 0;
    while(addr < length) {
        int bytes = (length - addr) > 16 ? 16 : (length - addr);
        fprintf(fp, ":%02X%04X00", bytes, addr);
        uint8_t checksum = bytes + (addr >> 8) + (addr & 0xFF);
        
        for(int i = 0; i < bytes; i++) {
            fprintf(fp, "%02X", data[addr + i]);
            checksum += data[addr + i];
        }
        
        fprintf(fp, "%02X\n", (uint8_t)(0x100 - checksum));
        addr += bytes;
    }
    
    fprintf(fp, ":00000001FF\n");
    fclose(fp);
    
    printf("INTEL HEX file written successfully: %s\n", filename);
    printf("  - File: %s\n", filename);
    printf("  - Size: %d bytes\n\n", length);

    return 1;
}

// Write raw hex dump file (no Intel HEX formatting) used for EPROM comparision.
int writeRawHexFile(const uint8_t* data, int length, const char* filename) {
    printf("Writing raw dump file: %s\n", filename);

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
        return 0; // Return 0 to indicate an error
    }

    // Write header with pattern information
    fprintf(fp, "// PRACTEL PT-430b 27C64-150 buffer dump\n");
    fprintf(fp, "// Size: %d bytes (0x%04X)\n", length, length);
    fprintf(fp, "// Format: Raw hex dump, 16 bytes per line\n");
    fprintf(fp, "//\n");
    fprintf(fp, "// Address Pattern Layout:\n");
    fprintf(fp, "// 0x0000-0x07FF: Pattern 1 - Color Bars (A11=0, A12=0) [CENTER POSITION]\n");
    fprintf(fp, "//   - 0x0000-0x007F: Initial pattern = Color Bar pattern\n");
    fprintf(fp, "//   - 0x0080-0x077F: Main pattern area Color Bar overlayed with ID\n");
    fprintf(fp, "//   - 0x0780-0x07FF: Line 16 pattern = Color Bar\n");
    fprintf(fp, "//\n");
    fprintf(fp, "// 0x0800-0x0FFF: Pattern 2 - Split Field Red (A11=1, A12=0) [RIGHT POSITION]\n");
    fprintf(fp, "//   - 0x0800-0x087F: Initial pattern= Color Bar pattern\n");
    fprintf(fp, "//   - 0x0880-0x0F7F: Main pattern area Color Bar overlayed with ID\n");
    fprintf(fp, "//   - 0x0F80-0x0FFF: Line 16 pattern = Red\n");
    fprintf(fp, "//\n");
    fprintf(fp, "// 0x1000-0x17FF: Pattern 3 - Pulse & Bar (A11=0, A12=1) [LEFT POSITION]\n");
    fprintf(fp, "//   - 0x1000-0x107F: Initial pattern = Color Bar pattern\n");
    fprintf(fp, "//   - 0x1080-0x177F: Main pattern area Color Bar overlayed with ID\n");
    fprintf(fp, "//   - 0x1780-0x17FF: Line 16 pattern = Pulse & Bar\n");
    fprintf(fp, "//\n");
    fprintf(fp, "// 0x1800-0x1FFF: Pattern 4 - Color Black (A11=1, A12=1) [NOT USED]\n");
    fprintf(fp, "//   - 0x1800-0x187F: Initial pattern = Color Black\n");
    fprintf(fp, "//   - 0x1880-0x1F7F: Main pattern area Color Black (NO ID Overlay)\n");
    fprintf(fp, "//   - 0x1F80-0x1FFF: Line 16 pattern = Color Black\n");
    fprintf(fp, "//\n");
    fprintf(fp, "// Addr   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    fprintf(fp, "//-------------------------------------------------------\n");

    int addr = 0;
    while (addr < length) {
          
        switch (addr) {
            case 0x0000:
                fprintf(fp, "\n// (0x0000-0x007F) Pattern 1 - Color Bars Initial Pattern\n\n");
                break;
            case 0x0080:
                fprintf(fp, "\n// (0x0080-0x077F) Pattern 1 - Color Bars Main Pattern with Text\n\n");
                break;
            case 0x0780:
                fprintf(fp, "\n// (0x0780-0x07FF) Pattern 1 - Color Bars Line 16\n\n");
                break;
            case 0x0800:
                fprintf(fp, "\n// (0x0800-0x087F) Pattern 2 - Split Field Bars Initial Pattern [Color Bars]\n\n");
                break;
            case 0x0880:
                fprintf(fp, "\n// (0x0880-0x08FF) Pattern 2 - Split Field Bars Main Pattern with Text [Color Bars]\n\n");
                break;
            case 0x0F80:
                fprintf(fp, "\n// (0x0F80-0x0FFF) Pattern 2 - Split Field Bars Line 16 [RED]\n\n");
                break;
            case 0x1000:
                fprintf(fp, "\n// (0x1000-0x107F) Pattern 3 - Pulse & Bar Initial Pattern [Color Bars]\n\n");
                break;
            case 0x1080:
                fprintf(fp, "\n// (0x1080-0x177F) Pattern 3 - Pulse & Bar Main Pattern with Text [Color Bars]\n\n");
                break;
            case 0x1780:
                fprintf(fp, "\n// (0x1780-0x17FF) Pattern 3 - Pulse & Bar Line 16 [Pulse & Bar]\n\n");
                break;
            case 0x1800:
                fprintf(fp, "\n// (0x1800-0x187F) Pattern 4 - Color Black Initial Pattern [Black]\n\n");
                break;
            case 0x1880:
                fprintf(fp, "\n// (0x1880-0x1F7F) Pattern 4 - Color Black Main Pattern no id overlay [Black]\n\n");
                break;
            case 0x1F80:
                fprintf(fp, "\n// (0x1F80-0x1FFF) Pattern 4 - Color Black Line 16 [Black]\n");
            }  

    fprintf(fp, "    %04X: ", addr);

    for (int i = 0; i < 16 && addr + i < length; i++) {
        fprintf(fp, "%02X ", data[addr + i]);
    }

    fprintf(fp, "  |");  // Separator between hex and ASCII

    for (int i = 0; i < 16 && addr + i < length; i++) {
        char c = data[addr + i];
        fprintf(fp, "%c", (isprint(c)) ? c : '.'); // Use isprint() for printable characters
    }

    fprintf(fp, "|\n");

    addr += 16;

}

fclose(fp);

printf("Raw dump written successfully:\n");
printf("  - File: %s\n", filename);
printf("  - Size: %d bytes\n\n", length);

return 1; // Return 1 to indicate success

}

// Print the Text Charater Bit Map for comparison.
int writeCharBitmapFile(const uint8_t* bitmap, const char* filename, const char* text) {
    if (!bitmap) {
        fprintf(stderr, "Error: Null bitmap pointer in writeCharBitmapFile\n");
        return 0;
    }
    
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
        return 0;
    }

    // Print out heading
    fprintf(fp, "Character bitmap for text: \"%s\"\n", text);
    fprintf(fp, "Dimensions: %d x %d\n", TEXT_BITMAP_WIDTH, TEXT_BITMAP_HEIGHT);
    fprintf(fp, "'X' represents a white id text pixel, '-' represents pattern background\n\n");
   
    // Print the bar/pixel position indicator line
    fprintf(fp, "Pixels : ");
    for (int pixel = 0; pixel < TEXT_BITMAP_WIDTH; pixel++) {
        fprintf(fp, "%c ", (pixel % 16 == 15) ? '|' : '-'); // '|' at every 16th pixel, '-' otherwise
    }
    fprintf(fp, "\n\n");

    // Print the text chars
    for (int line = 0; line < TEXT_BITMAP_HEIGHT; line++) {
        fprintf(fp, "Line %2d: ", line + 1); // Line numbers 1-7
        for (int pixel = 0; pixel < TEXT_BITMAP_WIDTH; pixel++) {
            uint8_t value = bitmap[line * TEXT_BITMAP_WIDTH + pixel];
            fprintf(fp, "%s", (value == COLOR_WHITE) ? "X " : "- "); // X for white, - for black
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    printf("\nDEBUG - Character bitmap written to: %s\n", filename);
    return 1;
}

// Print a EEPROM label with fancy boarders,
void printEpromLabel(const char* id_text, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
        return;
    }

    // Center the ID text
    int padding = (MAX_TEXT_LENGTH - strlen(id_text)) / 2;
    char padded_text[MAX_TEXT_LENGTH + 1];
    memset(padded_text, ' ', MAX_TEXT_LENGTH);
    size_t copy_len = strnlen(id_text, MAX_TEXT_LENGTH - padding);
    strncpy(padded_text + padding, id_text, copy_len);
    padded_text[MAX_TEXT_LENGTH] = '\0';

    // Get the current date and time
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    char date_string[11];
    strftime(date_string, sizeof(date_string), "%Y-%m-%d", local);

    // Calculate label width (for centering)
    int title_width = strlen("PT-430 COLORBAR-GEN");
    int id_width = strlen("ID: ") + strlen(padded_text);
    int date_width = strlen("Date: ") + strlen(date_string);
    int max_width = (title_width > id_width) ? (title_width > date_width ? title_width : date_width) : (id_width > date_width ? id_width : date_width);

     int cutout_padding = 4; // Adjust as needed
    max_width += 2 * cutout_padding;

      // HTML header and styling
    fprintf(fp, "<html>\n");
    fprintf(fp, "<head>\n");
    fprintf(fp, "<style>\n");
    fprintf(fp, "body { font-family: Helvetica, sans-serif; font-size: 4pt; text-align: center;}\n");
    fprintf(fp, ".label { border: 1px solid black; padding: 2px; text-align: center; display: inline-block; }\n");
    fprintf(fp, "</style>\n");
    fprintf(fp, "</head>\n");
    fprintf(fp, "<body>\n");

    // Label content within a div
    fprintf(fp, "<div class=\"label\">\n");
    fprintf(fp, "<b>PT-430 COLORBAR-GEN</b><br>\n"); // Bold title
    fprintf(fp, "ID: <b>%s</b><br>\n", padded_text); // Bold ID
    fprintf(fp, "Date: %s\n", date_string); // Date
    fprintf(fp, "</div>\n");

    fprintf(fp, "</body>\n");
    fprintf(fp, "</html>\n");

    fclose(fp);
    printf("EEPROM label written to: %s\n", filename);
}
