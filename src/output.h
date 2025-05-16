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

 output.h  include for output.c
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include "debug.h"
#include <stdint.h>

// File output functions
int writeHexFile(const uint8_t* data, int length, const char* filename);
int writeRawHexFile(const uint8_t* data, int length, const char* filename);
int writeBinFile(const uint8_t* data, int length, const char* filename);
int writeCharBitmapFile(const uint8_t* bitmap, const char* filename, const char* text);
void printEpromLabel(const char* id_text, const char* filename);

#endif // OUTPUT_H
