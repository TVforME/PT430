# PT430
PRACTEL PT-430 Color bar Generator EPROM Code generator.
The PRACTEL PT-430, developed in the early 1990s for the Australian broadcast industry, was primarily used in Electronic News Gathering (ENG) to transmit station IDs over microwave links.\
This repository aims to recreate the 27C64 EPROM code that generates the PT-430’s color bar and pulse-and-bar test patterns, including a center-aligned station ID. Through reverse engineering,\
I've determined that the EPROM utilizes two horizontal counters to sequentially output 4-bit GRBW data, controlling both the test pattern and embedded ID.

This project serves as a functional program to effectively generate the HEX code in the 27C64 EPROM or (28C64 EEPROM) to personalise the test patterns and identification text on the PT-430, enabling continued use in amateur television (ATV) applications.

## 8K EPROM addressing.
### Address Line Functions
A0–A6: Selects the first 140 lines of the pattern.

A7–A10: Selects 14 lines dedicated to the white ID overlay text.

A11–A12: Pattern selector via the front panel pattern selection toggle switch.

### Address Map Overview

| Address Range   | A12 | A11 | Description           | Usage                  |
|-----------------|-----|-----|-----------------------|------------------------|
| 0x0000–0x07FF   | 0   | 0   | Pattern 0 (2K)        | Reserved for expansion |
| 0x0800–0x0FFF   | 0   | 1   | Pattern 1 (2K)        | Active pattern (color bars) |
| 0x1000–0x17FF   | 1   | 0   | Pattern 2 (2K)        | Active pattern (pulse & bar) |
| 0x1800–0x1FFF   | 1   | 1   | Pattern 3 (2K)        | Reserved for expansion |

### Detailed Address Breakdown
For each 2K pattern block:

A0–A6 (7 bits): Selects one of 128 lines for the main pattern.

A7–A10 (4 bits): Selects one of 16 lines for the white ID overlay text.

A11–A12 (2 bits): Selects the pattern block.

This configuration allows for 128 main pattern lines and 16 overlay lines per pattern block.

### Data Line Assignments
D0: Green channel

D1: Red channel

D2: Blue channel

D3: White channel (used for overlay text)

### Pattern Selection Toggle Switch
The toggle switch connected to A11 and A12 allows selection between the two active pattern blocks:

Position 1: A11=0, A12=1 → Selects Pattern 1 (0x0800–0x0FFF)

Position 2: A11=1, A12=0 → Selects Pattern 2 (0x1000–0x17FF)

This setup ensures that only the inner 4K address space is utilized for active patterns, leaving the outer 4K (Patterns 0 and 3) available for future use.

Each pattern is a block of 2K switchable via address lines A11 and A12.
Technically the EPROM can actually address 4 patterns however A11 and A12 are connected to a simple toggle switch which configures swithing A11=1, A12=0 or either A11=0, A12=1 which addresses the inner 4K address space leaving the first and last 2K address spaces free for expansion.

## Pictures
Here's a working image of the output produced. PAL-BG 4:3  128 pixels across by 154 with odd and even field interlacing.

![PT430 Color bars](docs/images/PT430-Colorbars-w-ID.jpg)


