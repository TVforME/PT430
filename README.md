# PRACTEL PT-430 Color bar Generator EPROM Code generator.
The PRACTEL PT-430, were developed in the early 1990s for the Australian broadcast industry, and primarily used in (ENG) Electronic News Gathering to provide a constant video and audio between the station over a microwave link.

This project serves as a functional program to effectively generate the HEX code in the 27C64 EPROM to personalise the test patterns and identification text on the PT-430, enabling
continued use of the PT430 for amateur television (ATV) applications.

This repository's aim was to build a software tool to generate the necessary HEX code for a 27C64 (8K) since the DOS program was un-obtainable and probably long gone.
The generator produce 3 working patterns color black, color bars and pulse-and-bar test patterns. Color bars and pulse-and-bar produced a center-aligned station ID.


# May 2025.
After dumping the EPROM HEX code, and navigating the schematic, the PT430 can effectively only generate primary/secondary adaptive colors with 0x00 Black and 0xFF White.\
Cascaded 4 bit counters (74HC393) count 7-bits horizontally clocking out the same line for 128 pixels , then once at line 140, derived from vertical counter a 4-bit binary up counter (4520) takes over clocking out 16 lines of odd/even lines of either color bars of color black pattern overlayed with text id chars bitmap pixels for 7 lines (5x7 font) of which repeat for odd/even fields (14 lines) then is held at reset at line 156 repeating the last line 16 till the end of the next frame awaiting another vertical interval.

## 27C64 8K EPROM addressing as I see it.
### Address Map Overview
A0–A6: (7 bits) Selects the first 140 lines of the pattern, a cascaded 4bit binary ripple counter 74HC393(IC7) clocked at 5Mhz to form the pixel clock.

A7–A10: (4 bits) Selects 14 lines dedicated to the white ID overlay text counter CD4520 (IC5:B) binary up counter.

A11–A12: (2 bits) Pattern selector via the front panel pattern selection toggle switch.

7 bits translates to a 2K pattern blocks addressing effectively 4 patterns:

| Address Range   | A12 | A11 | Description           | Pattern                       |
|-----------------|-----|-----|-----------------------|-------------------------------|
| 0x0000-0x000F   |  X  |  X  |  Not - used Q0        | Not used 0x0F                 |
| 0x0010–0x07FF   | 0   | 0   | Pattern 0 (2K)        | Pattern (color bars)          |
| 0x0810–0x0FFF   | 0   | 1   | Pattern 1 (2K)        | Pattern (split filed bars)    |
| 0x1010–0x17FF   | 1   | 0   | Pattern 2 (2K)        | Pattern (pulse & bar)         |
| 0x1810–0x1FFF   | 1   | 1   | Pattern 3 (2K)        | Patter (color black)          |

Pattern Layout per block of 2K:

| Address Range   | Pattern cycle                                           | Function
|-----------------|---------------------------------------------------------|----------------------------------------|
|    Pattern 1    |                                                         |                                        |
| 0x0010-0x008F   | Initial color bars (8 bars each of 16 pixels)           |  Iterates over 1 x 128 pixels          |
| 0x0090-0x078F   | Main pattern with text ID over color bars               |  odd/even field, 14 lines x 128 pixels |
| 0x0790-0x07FF   | Last pattern color bars (8 bars each of 16 pixels)      |  Iterates over 1 x 128 pixels          |
|    Pattern 2    |                                                         |                                        |
| 0x0810-0x088F   | Initial color bar (8 bars each of 16 pixels)            |  Iterates over 1 x 128 pixels          |
| 0x0890-0x0F8F   | Main pattern with text ID over color bars               |  odd/even field, 14 lines x 128 pixels |
| 0x0F90-0x0FFF   | Last pattern split bars (128 of Red pixels)             |  Iterates over 1 x 128 pixels          |
|    Pattern 3    |                                                         |                                        |
| 0x1010-0x108F   | Initial color bar (8 bars each of 16 pixels)            |  Iterates over 1 x 128 pixels          |
| 0x1090-0x178F   | Main pattern with text ID over color bars               |  odd/even field, 14 lines x 128 pixels |
| 0x1790-0x17FF   | Last pattern pulse & bar ( pulse 1 pixel, bar 8 pixels) |  Iterates over 1 x 128 pixels          |
|    Pattern 4    |                                                         |                                        |
| 0x1810-0x188F   | Initial color black (128 pixels of black)               |  Iterates over 1 x 128 pixels          |
| 0x1090-0x178F   | Main pattern  no text  14 x 128 pixels of balck         |  odd/even field, 14 lines x 128 pixels |
| 0x1790-0x17FF   | Last pattern color black  (128 of Black pixels)         |  Iterates over 1 x 128 pixels          |

### EPROM Dataline Assignments:

| Data line  | channel     |
|------------|-------------|
| D0         |    GREEN    |
| D1         |    RED      |
| D2         |    BLUE     |
| D3         |    WHITE    |

Note - Although the combination of D0 || D1 || D2 make white, D3 is used as an independant white signal to cancel out effects of 100% white.
The white is added to the red and blue matrix in the U-V modulator.

Each of color outputs are latched by a 74HC273 (IC8) to provide a stable digital RGBW to the luminace and U-V matrix. 

### Pattern Selection Toggle Switch
The toggle switch connected to A11 and A12 allows selection between the two active pattern blocks:

Position 1: A11=0, A12=1 → Selects Pattern 1 (0x0800–0x0FFF)

Position 2: A11=0, A12=0 → Selects Pattern 2 (0x0000–0x07FF)

Position 3: A11=1, A12=0 → Selects Pattern 3 (0x1000–0x17FF)

Position 4: A11=1, A12=1 → Selects Pattern 4 (0x1000–0x17FF)  (Not usable without modification of the front toggle switch)

## Pictures
Here's a working image of the output with pattern 1 selected using the original EPROM.
PAL-BG 4:3

![PT430 Color bars](docs/images/PT430-Colorbars-w-ID.jpg)


