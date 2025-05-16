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
 
tcgen.c  main  for PT-430 color bar generator, calls all other modules.

*/

// Local includes
#include "tcgen.h"
#include "debug.h"
#include "patterns.h"
#include "version.h"
#include "output.h"

// Library includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <libgen.h>

// set debug to false.
bool debug_enabled = false;

// Function to validate if a character is in our allowed set
int isValidChar(char c) {
    c = toupper(c);
    return (c == ' ' || c == '-' || c == ':' || 
            (c >= 'A' && c <= 'Z') || 
            (c >= '0' && c <= '9'));
}


// Function to validate the entire text string
int validateText(const char* text) {
    if (strlen(text) > MAX_TEXT_LENGTH) {
        fprintf(stderr, "Error: Text length exceeds maximum of %d characters\n", MAX_TEXT_LENGTH);
        return 0;
    }
    
    for(int i = 0; text[i]; i++) {
        if (!isValidChar(text[i])) {
            fprintf(stderr, "Error: Invalid character '%c' at position %d\n", text[i], i);
            fprintf(stderr, "Allowed characters are A-Z, 0-9, space, hyphen (-) and colon (:)\n");
            return 0;
        }
    }
    return 1;
}

// Extract year from __DATE__ macro (format: "Mmm DD YYYY")
const char* getYearFromDate(const char* date) {
    // Return last 4 characters of __DATE__ string
    return &date[7];
}

void printVersion(void) {
    printf("PRACTEL PT-430b EPROM Code Generator v%s\n", VERSION_STRING);
    printf("Build date: %s %s\n", BUILD_DATE, BUILD_TIME);
    printf("Copyright (c) %s Robert Hensel VK3DG email:vk3dgtv@gmail.com\n",getYearFromDate(BUILD_DATE));
}

// Print Usage
void printUsage(const char* progname) {
    fprintf(stderr, "Usage: %s [-v] [-d] [-h] -t <text> -o <output file>\n", progname);
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "  -v           Show version information\n");
    fprintf(stderr, "  -d           Enable debug mode - Prints additional files, <name>.dump\n");
    fprintf(stderr, "                                   1. Raw hex dump with ASCII file\n");
    fprintf(stderr, "                                   2. Char map char-bitmap.txt file\n");
    fprintf(stderr, "  -h           Show this help message\n");
    fprintf(stderr, "  -t <text>    Text to display (A-Z, 0-9, space, - and :)\n");
    fprintf(stderr, "  -o <file>    Output file name (.hex will be created, .bin for binary)\n");
    fprintf(stderr, "\nExample:\n");
    fprintf(stderr, "  %s -t \"VK3DG GEELONG\" -o pattern.hex\n", progname);
    fprintf(stderr, "\nOutputs:\n");
    fprintf(stderr, "  <name>.hex   Intel HEX format file\n");
    fprintf(stderr, "  <name>.bin   Binary format file\n");
    fprintf(stderr, "  <name>.dump  Raw hex dump with ASCII\n");
}

// main entry point of program.
int main(int argc, char *argv[]) {

    char id_text[MAX_TEXT_LENGTH + 1] = {0}; // Initialize to empty string
    char output_file[256] = {0};             // Initialize to empty string
    int opt;

    // Parse command line options
    while ((opt = getopt(argc, argv, "t:o:hvd")) != -1) {
        switch (opt) {
            case 't':
            {
                char* text_arg = optarg;
                size_t len = strlen(text_arg);
                
                // Remove surrounding quotes if present
                if (len >= 2) {
                    if ((text_arg[0] == '"' && text_arg[len-1] == '"') ||
                        (text_arg[0] == '\'' && text_arg[len-1] == '\'')) {
                        text_arg[len-1] = '\0';
                        text_arg++;
                        len -= 2;
                    }
                }
                
                // Convert underscores to spaces
                for(int i = 0; i < len; i++) {
                    if(text_arg[i] == '_') text_arg[i] = ' ';
                }
                strncpy(id_text, text_arg, sizeof(id_text) - 1);
                id_text[sizeof(id_text) - 1] = '\0';
            }
            break;
            case 'o':
                strncpy(output_file, optarg, sizeof(output_file) - 1);
                output_file[sizeof(output_file) - 1] = '\0'; // Ensure null-termination
                break;
            case 'v': // Version option
                printVersion();
                return 0;
            case 'h':
                printUsage(argv[0]);
                return 0;
            case 'd':
                debug_enabled = true;  // Set debug flag
                break;
            default: /* '?' */
                printUsage(argv[0]);
                return 1;
        }
    }

    // Spit out app name
    printf("\nPRACTEL PT-430b EPROM Code Generator v%s\n", VERSION_STRING);
    
    // Check if all required parameters are provided
    if (!id_text[0] || !output_file[0]) { // Check if strings are empty
        fprintf(stderr, "Error: Missing required parameters\n");
        printUsage(argv[0]);
        return 1;
    }

    // Validate text length and correct characters
    if (!validateText(id_text)) {
        return 1;
    }

     // Dynamic memory allocation (improved)
     uint8_t* eprom_data = NULL;
     uint8_t* bitmap_data = NULL;
 
     eprom_data = (uint8_t*)calloc(EPROM_SIZE, sizeof(uint8_t));
     if (!eprom_data) {
         perror("Error allocating memory for EPROM data"); // Use perror for system error messages
         return 1; // Return an error code
     }
 
     bitmap_data = (uint8_t*)calloc(PIXELS_PER_LINE * TEXT_BITMAP_HEIGHT, sizeof(uint8_t));
     if (!bitmap_data) {
         perror("Error allocating memory for bitmap data");
         free(eprom_data); // Free previously allocated eprom memory
         return 1;
     }

    // Generate our pattern buffer data
    printf("\nGenerating EPROM data for ID Text %s\n", id_text);

    if (!generateEpromData(eprom_data, bitmap_data, id_text)) {
        fprintf(stderr, "Error: Pattern generation failed\n");
        freeMemory(eprom_data, bitmap_data);
        return 1;
    }

    // Success!
    printf("\nPattern generation completed successfully\n");

    char* base_filename = NULL; // Use basename for better portability
    base_filename = strdup(output_file); // Allocate memory for base_filename
    if (!base_filename) {
        perror("Error duplicating filename");
        freeMemory(eprom_data, bitmap_data);
        return 1;
    }

    // Remove file extension if happen to have one.
    char* ext = strrchr(base_filename, '.');
    if (ext) {
        *ext = '\0'; // Truncate the string at the extension
    }

    char* dir_name = NULL; // Get directory name (if needed)
    dir_name = strdup(output_file);
    if (!dir_name) {
        perror("Error duplicating filename for directory");
        free(base_filename);
        freeMemory(eprom_data, bitmap_data);
        return 1;
    }

    base_filename = basename(base_filename); // Extract filename only
    dir_name = dirname(dir_name); // Extract directory name

    char hex_filename[256];
    char bin_filename[256];
    char dump_filename[256];
    char text_filename[256];
    char label_filename[256];
   
  
    snprintf(hex_filename, sizeof(hex_filename), "%s/%s.hex", dir_name, base_filename);
    snprintf(bin_filename, sizeof(bin_filename), "%s/%s.bin", dir_name, base_filename);
    snprintf(dump_filename, sizeof(dump_filename), "%s/%s.dump", dir_name, base_filename);
    snprintf(text_filename, sizeof(text_filename), "%s/%s_id.txt", dir_name, base_filename); 
    snprintf(label_filename, sizeof(label_filename), "%s/%s_eprom_label.html", dir_name, base_filename); 


   // Write INTEL HEX file
   if (!writeHexFile(eprom_data, EPROM_SIZE, hex_filename)) {
       fprintf(stderr, "Error: Failed to write HEX file: %s\n", hex_filename);
       freeMemory(eprom_data, bitmap_data);
       return 1;
   }

   // Write BIN file
   if (!writeBinFile(eprom_data, EPROM_SIZE, bin_filename)) {
       fprintf(stderr, "Error: Failed to write binary file: %s\n", bin_filename);
       freeMemory(eprom_data, bitmap_data);
       return 1;
   }
   
   if (debug_enabled) {
        // Write .dump file for comparison purposes.
        if (!writeRawHexFile(eprom_data, EPROM_SIZE, dump_filename)) {
            fprintf(stderr, "Error: Failed to write raw hex dump: %s\n", dump_filename);
            freeMemory(eprom_data, bitmap_data);
            return 1;
        } else {
            printf("- Raw hex dump: %s\n", dump_filename);
        }
    }

  // In debug print Text bitmap file
    if (debug_enabled) {
        if (!writeCharBitmapFile(bitmap_data, text_filename, id_text)) {
            fprintf(stderr, "Error writing character bitmap file.\n");
            freeMemory(eprom_data, bitmap_data);
            return 1;
         }
    }

   // Print EPROM label
   printEpromLabel(id_text, label_filename);
   
   printf("- EPROM size: %d bytes\n", EPROM_SIZE);

   printf("\n(E)EPROM pattern file completed successfully:\n");

   freeMemory(eprom_data, bitmap_data);
   return 0;
}

 // De-allocate the 8K buffer from memory and text bitmap buffer
void freeMemory(uint8_t* eprom, uint8_t* bitmap){
    if (eprom) {
        free(eprom);
    }
    if (bitmap) {
      
        free(bitmap);
    }
}
