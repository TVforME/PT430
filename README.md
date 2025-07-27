# PRACTEL PT-430 Color bar Generator EPROM Code generator.
The [PRACTEL](docs/practel_info.md) PT-430, were developed in the early 1990s for the Australian broadcast industry, and primarily used in (ENG) Electronic News Gathering to provide a constant video and audio between the station over a microwave link.

<table align="center">
<tr>
  <td width="25%" align="center">
    <img src="https://github.com/TVforME/PT430/blob/main/docs/images/PT430-Frontview.jpg" width="60%">
    <br>
    Front
  </td>
  <td width="25%" align="center">
    <img src="https://github.com/TVforME/PT430/blob/main/docs/images/PT430-Rearview.jpg" width="60%">
    <br>
    Rear
  </td>
  <td width="25%" align="center">
    <img src="https://github.com/TVforME/PT430/blob/main/docs/images/PT430-Videoboard.jpg" width="60%">
    <br>
    Video Board
  </td>
  <td width="25%" align="center">
    <img src="https://github.com/TVforME/PT430/blob/main/docs/images/PT430-Audioboard.jpg" width="60%">
    <br>
    Audio/switcher board
  </td>
</tr>
</table>


This project is my suuccessful reverse Engineering of the code within the EPROM and developing a C99 program to re-generate both the patterns and ID text overly for the purpose of personalise the test patterns and identification text on the PT-430.
There remains a few PT-430 generators around with the knowledge that not all is lost and continued using the PT430 for amateur television (ATV) applications. 
4:3 PAL BG format is old skool however, there maybe instances for using and converting the CVBS output into DVB with some cheap converters. I know both the SAA1043 and SAA1044 philips sync generator and subcarrier coupler were well known to be used in top end cameras and studio quality sync pulse generators.

The reverse Engineering continues to hone my skills of understanding the addressing and developing a C program for windows and linux with the linux build yet untested.

# Moreover:-
The original EPROM program I believe was written in GW Basic unfortunately, now un-obtainium the original disk is more than likely gone for ever stashed with the DOS 6.1 disk set?

The generator produce 3 working patterns in 4:3 aspect ratio of color black, color bars and pulse-and-bar test patterns. Both the Color bars and pulse-and-bar produced a center-aligned station ID.
My program generate 4 patterns the 4th pattern not able to be displayed due to the use of a center-off-toggle switch. If the switch is replaced with the 4 position selector switch, the 4th pattern is available for free?
After dumping the EPROM HEX code, and navigating the schematic, the PT430 can effectively only generate primary/secondary adaptive colors from 0x00 Black through to 0x0F white with combibations of the pattern on the lower nibble D0 to D3 on the EPROM data lines.

Referrencing to the [PT-430 schematic](docs/PRACTEL%20PT430%20Colorbar%20Generator.pdf)

Cascaded 4 bit counters (74HC393) count 7-bits horizontally clocking out the same line for 128 pixels , then once at line 140, derived from vertical counter a 4-bit binary up counter (4520) takes over clocking out 16 lines of odd/even lines of either color bars of color black pattern overlayed with text id chars bitmap pixels for 7 lines (5x7 font) of which repeat for odd/even fields (14 lines) then is held at reset at line 156 repeating the last line 16 till the end of the next frame awaiting another vertical interval.

## 27C64 8K EPROM addressing as I see it.
### Address Map Overview
A0–A6: (7 bits) Selects the first 140 lines of the pattern, a cascaded 4bit binary ripple counter 74HC393(IC7) clocked at 5Mhz to form the pixel clock.

A7–A10: (4 bits) Selects 14 lines dedicated to the white ID overlay text counter CD4520 (IC5:B) binary up counter.

A11–A12: (2 bits) Pattern selector via the front panel pattern selection toggle switch.

7 bits translates to a 2K pattern blocks addressing effectively 4 patterns. 
The 4th pattern is not used without modification to the center-of-toggle switch:

| Address Range   | A12 | A11 | Description           | Pattern                        |
|-----------------|-----|-----|-----------------------|--------------------------------|
| 0x0000–0x07FF   | 0   | 0   | Pattern 0 (2K)        | Pattern (color bars)           |
| 0x0800–0x0FFF   | 0   | 1   | Pattern 1 (2K)        | Pattern (split filed red bars) |
| 0x1000–0x17FF   | 1   | 0   | Pattern 2 (2K)        | Pattern (pulse & bar)          |
| 0x1800–0x1FFF   | 1   | 1   | Pattern 3 (2K)        | Patter (color black)           |

Pattern Layout per block of 2K:

| Address Range   | Pattern cycle                                           | Function
|-----------------|---------------------------------------------------------|----------------------------------------|
|    Pattern 1    |                                                         |                                        |
| 0x0000-0x008F   | Initial color bars (8 bars each of 16 pixels)           |  Iterates over 1 x 128 pixels          |
| 0x0080-0x077F   | Main pattern with text ID over color bars               |  odd/even field, 14 lines x 128 pixels |
| 0x0780-0x07FF   | Last pattern color bars (8 bars each of 16 pixels)      |  Iterates over 1 x 128 pixels          |
|    Pattern 2    |                                                         |                                        |
| 0x0800-0x087F   | Initial color bar (8 bars each of 16 pixels)            |  Iterates over 1 x 128 pixels          |
| 0x0880-0x0F7F   | Main pattern with text ID over color bars               |  odd/even field, 14 lines x 128 pixels |
| 0x0F80-0x0FFF   | Last pattern split bars (128 of Red pixels)             |  Iterates over 1 x 128 pixels          |
|    Pattern 3    |                                                         |                                        |
| 0x1000-0x107F   | Initial color bar (8 bars each of 16 pixels)            |  Iterates over 1 x 128 pixels          |
| 0x1080-0x177F   | Main pattern with text ID over color bars               |  odd/even field, 14 lines x 128 pixels |
| 0x1780-0x17FF   | Last pattern pulse & bar ( pulse 1 pixel, bar 8 pixels) |  Iterates over 1 x 128 pixels          |
|    Pattern 4    |                                                         |                                        |
| 0x1800-0x187F   | Initial color black (128 pixels of black)               |  Iterates over 1 x 128 pixels          |
| 0x1080-0x177F   | Main pattern  no text  14 x 128 pixels of balck         |  odd/even field, 14 lines x 128 pixels |
| 0x1780-0x17FF   | Last pattern color black  (128 of Black pixels)         |  Iterates over 1 x 128 pixels          |

### EPROM Dataline Assignments:

| Data line  | channel     |
|------------|-------------|
| D0         |    GREEN    |
| D1         |    RED      |
| D2         |    BLUE     |
| D3         |    WHITE    |

Note - Although the combination of D0 AND D1 AND D2 make white, D3 is used as an independant white signal to cancel out effects of 100% white.
The white is added to the red and blue matrix in the U-V modulator.

Each of color outputs are latched by a 74HC273 (IC8) to provide a stable digital RGBW to the luminace and U-V matrix. 

### Pattern Selection Toggle Switch
The toggle switch connected to A11 and A12 allows selection between the two active pattern blocks:

Position 1: A11=0, A12=1 → Selects Pattern 1 (0x0800–0x0FFF)

Position 2: A11=0, A12=0 → Selects Pattern 2 (0x0000–0x07FF)

Position 3: A11=1, A12=0 → Selects Pattern 3 (0x1000–0x17FF)

Position 4: A11=1, A12=1 → Selects Pattern 4 (0x1000–0x17FF)  (Not usable without modification of the front toggle switch as noted.)

## Pictures
Here's the pictures of the 4 patterns with the 1st picture showing the original id overlay before generation of new patterns.

<table align="center">
<tr>
  <td width="25%" align="center">
    <img src="https://github.com/TVforME/PT430/blob/main/docs/images/PT430-Colorbars-w-id.jpg" width="60%">
    <br>
    Original ID
  </td>
  <td width="25%" align="center">
    <img src="https://github.com/TVforME/PT430/blob/main/docs/images/PT430-Colorbars-vk3xka.jpg" width="60%">
    <br>
    Color Bars
  </td>
  <td width="25%" align="center">
    <img src="https://github.com/TVforME/PT430/blob/main/docs/images/PT430-Splitbars-vk3xka.jpg" width="60%">
    <br>
    Split Field Red Bars
  </td>
  <td width="25%" align="center">
    <img src="https://github.com/TVforME/PT430/blob/main/docs/images/PT430-Pulsebars-vk3xka.jpg" width="60%">
    <br>
    Pulse & Bar
  </td>
</tr>
</table>

## How to build form [sourcecode](docs/build/build.md)



