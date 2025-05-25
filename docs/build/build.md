# Setup Development Environment in Windows 10/11

## Prerequisites

### 2. MSYS2
Download and install MSYS2 from [https://www.msys2.org/](https://www.msys2.org/)

## Installation Steps

### 1. Install GCC Tools
Open MSYS2 UCRT64 terminal and run:
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
```
Now add gcc to windows path.
open environment variables, navigate to PATH for current user -> New path
browse to:- C:\msys2\urct46\bin
Add and save. 

### 2. Install GCC Make
Open MSYS2 UCRT64 terminal and run:
```bash
pacman -S mingw-w64-x86_64-make
```
Now add mingw32-make to windows path.
open environment variables, navigate to PATH for current user -> New path
browse to:- C:\msys2\msys64\bin
Add and save. 

### 3. Download source files.
Either git clone https://github.com/TVforME/PT430.git
OR 
Download the zip file and unzip to folder PT430-main
If downloading zip file, rename PT430-main to PT430

### 4. Building TCGEN.exe
Note: with Linux use build.sh (not tested) chmod +x build.sh first.
Using cmd navigate to where you unziped of copied your PT430 folder to.
```bash
cd <YOULOCATION>/PT430/src
build.bat
```
Should build without errors with the build output below
```text
C:\Projects\PT430\src>build.bat
Removed existing version.h
Generating version.h...
New version.h generated
Building on Windows...
if exist bin rmdir /s /q bin
if exist build rmdir /s /q build
"Build files cleaned"
if not exist "bin" mkdir "bin"
if not exist "build" mkdir "build"
gcc -Wall -O2 -I. -c tcgen.c -o build\\tcgen.o
gcc -Wall -O2 -I. -c patterns.c -o build\\patterns.o
gcc -Wall -O2 -I. -c output.c -o build\\output.o
gcc -o bin\\tcgen.exe build\\tcgen.o build\\patterns.o build\\output.o
Build successful
Version: 1.0.0 (Sun 05/25/2025 20:36:11.30
Copyright (c) 2025
```
now cd into /bin
```bash
cd bin
tcgen -t "vk3eht" -o vk3eht
```
tcgen.exe creates EPROM output files ready for loading into EPROM programmer.

### Example without debug
C:\Projects\PT430\src\bin>tcgen -t "vk3eht" -o vk3eht
```text
PRACTEL PT-430b EPROM Code Generator v1.0.0

Generating EPROM data for ID Text vk3eht

Pattern generation completed successfully
Writing INTEL HEX file: ./vk3eht.hex
INTEL HEX file written successfully: ./vk3eht.hex
  - File: ./vk3eht.hex
  - Size: 8192 bytes

Writing binary file: ./vk3eht.bin
Binary file written successfully:
  - File: ./vk3eht.bin
  - Size: 8192 bytes
EEPROM label written to: ./vk3eht_eprom_label.html
- EPROM size: 8192 bytes

(E)EPROM pattern file completed successfully:
 ```
#### Alternively, if needing the debug files
```bash
tcgen -d -t "vk3eht" -o vk3eht
```
 C:\Projects\PT430\src\bin>tcgen -d -t "vk3eht" -o vk3eht
```text
PRACTEL PT-430b EPROM Code Generator v1.0.0

Generating EPROM data for ID Text vk3eht

Pattern generation completed successfully
Writing INTEL HEX file: ./vk3eht.hex
INTEL HEX file written successfully: ./vk3eht.hex
  - File: ./vk3eht.hex
  - Size: 8192 bytes

Writing binary file: ./vk3eht.bin
Binary file written successfully:
  - File: ./vk3eht.bin
  - Size: 8192 bytes
Writing raw dump file: ./vk3eht.dump
Raw dump written successfully:
  - File: ./vk3eht.dump
  - Size: 8192 bytes

- Raw hex dump: ./vk3eht.dump

DEBUG - Character bitmap written to: ./vk3eht_id.txt
EEPROM label written to: ./vk3eht_eprom_label.html
- EPROM size: 8192 bytes

(E)EPROM pattern file completed successfully:
```




