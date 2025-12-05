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
---

# ✨ Project Overview

This project showcases my successful reverse engineering of the code stored in the EPROM of the PT-430. I developed a C program capable of regenerating both the test patterns and the ID text overlays, allowing for personalized test patterns and identification text on the PT-430 generator.

---

## 📺 Keeping the PT-430 Alive

A handful of PT-430 generators are still in use, proving that not all classic tech has faded away! Thanks to this work, the PT-430 continues to be a valuable tool for amateur television (ATV) enthusiasts. While the 4:3 PAL BG format may seem "old school," there are still scenarios where its CVBS output can be converted to DVB using affordable converters.

---

## 🎛️ A Nod to Classic Hardware

I have a special appreciation for the **SAA1043** and **SAA1044** Philips sync generator and subcarrier coupler chips. These components were renowned for their use in high-end cameras and studio-grade sync pulse generators—a testament to their quality and reliability.

---

## 🛠️ Growing My Skills

The reverse engineering journey has sharpened my understanding of memory addressing and embedded systems. I’ve also created a C program that runs on both Windows and Linux (though the Linux build is still awaiting its first test). This ongoing project continues to challenge and expand my technical abilities.

---

Here’s your content, clearly structured and enhanced in Markdown for readability:

---

## ⚙️ How It Works

The original EPROM program for the PT-430 was likely written in GW-BASIC. Unfortunately, the disk containing the source code is now “un-obtainium”—it’s probably lost forever, possibly tucked away with an old DOS 6.1 disk set!

### 🖼️ Test Pattern Generation

- The PT-430 generator produces **three working patterns** in a 4:3 aspect ratio:
  - **Color Black**
  - **Color Bars**
  - **Pulse-and-Bar** test patterns
- Both the Color Bars and Pulse-and-Bar patterns feature a **center-aligned station ID**.
- **My program generates four patterns**—however, the 4th pattern can't be displayed without modifying the hardware. The original uses a center-off toggle switch; replacing it with a 4-position selector switch unlocks the 4th pattern for free!

### 🧩 EPROM Dump & Pattern Logic

- After dumping the EPROM hex code and studying the schematic, I found:
  - The PT-430 can generate **primary/secondary adaptive colors** from `0x00` (Black) up to `0x0F` (White).
  - Pattern combinations are determined by the lower nibble (`D0` to `D3`) on the EPROM data lines.

> 📄 **Reference:** [PT-430 schematic](docs/PRACTEL%20PT430%20Colorbar%20Generator.pdf)

---

## ⏱️ Timing & Pattern Rendering Details

- **Cascaded 4-bit counters** (`74HC393`) count 7 bits horizontally.
  - This clocks out the same line for **128 pixels**.
- At **line 140**, a **4-bit binary up counter** (`CD4520`) takes over, clocking out 16 lines:
  - Odd/even lines display either color bars or color black.
  - Text ID character bitmaps (5x7 font) are overlayed for 7 lines, repeating for odd/even fields (14 lines).
  - At **line 156**, the system is held at reset, repeating the last line 16 times until the next frame, awaiting another vertical interval.

---

## 💾 27C64 8K EPROM Addressing

### 🗺️ Address Map Overview

- **A0–A6 (7 bits):**  
  Selects the first 140 lines of the pattern.  
  - Driven by a cascaded 4-bit binary ripple counter (`74HC393`, IC7) clocked at 5 MHz for the pixel clock.

- **A7–A10 (4 bits):**  
  Selects 14 lines dedicated to the white ID overlay text.  
  - Managed by a binary up counter (`CD4520`, IC5:B).

- **A11–A12 (2 bits):**  
  Pattern selector via the front panel pattern selection toggle switch.

**Summary:**  
- 7 bits = 2K pattern block addressing, effectively supporting **4 patterns**.
- The 4th pattern is not used unless the center-off toggle switch is replaced with a 4-position selector.

---

### 🗺️ Address Map Table

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



