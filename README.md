# PT430
PRACTEL PT-430 Color bar Generator EPROM Code generator.
The PRACTEL PT-430, developed in the early 1990s for the Australian broadcast industry, was primarily used in Electronic News Gathering (ENG) to transmit station IDs over microwave links.\
This repository aims to recreate the 27C64 EPROM code that generates the PT-430’s color black, color bars and pulse-and-bar test patterns, including a center-aligned station ID. Through reverse engineering,\
I've determined that the EPROM utilizes two horizontal counters to sequentially output a 7-bit and a second 4-bit binary counter to sequence the correct GRBW data.\
What's inside the EPROM is a mystery 🕵️ which has lead to reverse Engineering the HEX code inside the 27C64 to find out how and can we implement both the pattern sequence as well as the text identification.

This project serves as a functional program to effectively generate the HEX code in the 27C64 EPROM or (28C64 EEPROM) to personalise the test patterns and identification text on the PT-430, enabling\
continued use of the PT430 for amateur television (ATV) applications.

## 27C64 8K EPROM addressing as I see it.
### Address Map Overview
A0–A6: (7 bits) Selects the first 140 lines of the pattern, a cascaded 4bit binary ripple counter 74HC393(IC7) clocked at 5Mhz to form the pixel clock.

A7–A10: (4 bits) Selects 14 lines dedicated to the white ID overlay text counter CD4520 (IC5:B) binary up counter.

A11–A12: (2 bits) Pattern selector via the front panel pattern selection toggle switch.

7 bits translates to a 2K pattern blocks addressing effectively 4 patterns:

| Address Range   | A12 | A11 | Description           | Pattern                       |
|-----------------|-----|-----|-----------------------|-------------------------------|
| 0x0000-0x000F   |  X  |  X  |  Not - used Q0        | Not used 0x0F                 |
| 0x0010–0x07FF   | 0   | 0   | Pattern 0 (2K)        | Pattern (color black)         |
| 0x0810–0x0FFF   | 0   | 1   | Pattern 1 (2K)        | Pattern (split filed bars)    |
| 0x1010–0x17FF   | 1   | 0   | Pattern 2 (2K)        | Pattern (pulse & bar)         |
| 0x1810–0x1FFF   | 1   | 1   | Pattern 3 (2K)        | Patter (color black)          |

Pattern Layout per block of 2K:

| Address Range   | Pattern cycle                   | Function
|-----------------|---------------------------------|-------------------------------|
| 0x0010-0x008F   | Initial color bar pattern       |  Iterates over 128 pixels     |
| 0x0090-0x078F   | Main pattern with text overlay  |  odd/even field 14 lines      |
| 0x0790-0x07FF   | Line 16 pattern                 |   Iterates over 128 pixels    |

### Data Line Assignments
D0: Green channel
D1: Red channel
D2: Blue channel
D3: White channel

Note - Although the combination of D0 || D1 || D2 make white, D3 is used as an independant white signal to cancel out effects of 100% white.
The white is added to the red and blue in the U-V modulator.

The White channel (D3) should be active when both Red (D1) and Blue (D2) channels are active.\
This can be represented by the logical AND operation:  D3 = D1 AND D2

Each of color output is latched at 2.5Mhz using 74HC273 (IC8) to provide a stable digital RGBW timed on the 2.5Mhx boundry.
Required reduce jitter and drive the luminance matrix in addition to the U & V color modulators.

### Pattern Selection Toggle Switch
The toggle switch connected to A11 and A12 allows selection between the two active pattern blocks:

Position 1: A11=0, A12=1 → Selects Pattern 1 (0x0800–0x0FFF)

Position C: A11=0, A12=0 → Selects Pattern 0 (0x0000–0x07FF)

Position 2: A11=1, A12=0 → Selects Pattern 2 (0x1000–0x17FF)

Each pattern is a block of 2K switchable via address lines A11 and A12.

## Pictures
Here's a working image of the output with pattern 1 selected using the original EPROM.
PAL-BG 4:3

![PT430 Color bars](docs/images/PT430-Colorbars-w-ID.jpg)


