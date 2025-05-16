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

 tcgen.h include file..
 */

#ifndef TCGEN_H
#define TCGEN_H

#include "patterns.h"
#include <stdint.h>

// Function declarations
int isValidChar(char c);
int validateText(const char* text);
void printUsage(const char* progname);
void freeMemory(uint8_t* eprom, uint8_t* bitmap);

#endif // TCGEN_H
