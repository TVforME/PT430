# PT430
PRACTEL PT-430 Color bar Generator EPROM Code generator.
The PRACTEL PT-430, developed in the early 1990s for the Australian broadcast industry, was primarily used in Electronic News Gathering (ENG) to transmit station IDs over microwave links.\
This repository aims to recreate the 27C64 EPROM code that generates the PT-430’s color bar and pulse-and-bar test patterns, including a center-aligned station ID. Through reverse engineering,\
I've determined that the EPROM utilizes two horizontal counters to sequentially output 4-bit GRBW data, controlling both the test pattern and embedded ID.

This project serves as a functional program to effectively generate the HEX code in the 27C64 EPROM or (28C64 EEPROM) to personalise the test patterns and identification text on the PT-430, enabling continued use in amateur television (ATV) applications.

## 8K EPROM addressing.
A0-A6 addresses the first 140 lines of the pattern.
A7-A10 address on a seperate 14 lines for the white ID overlay text.
D0=Green
D1=Red
D2=Blue
D3=White

Each pattern is a block of 2K switchable via address lines A11 and A12.
Technically the EPROM can actually address 4 patterns however A11 and A12 are connected to a simple toggle switch which configures swithing A11=1, A12=0 or either A11=0, A12=1 which addresses the inner 4K address space leaving the first and last 2K address spaces free for expansion.


## Pictures
Here's a working image of the output produced. PAL-BG 4:3  128 pixels across by 154 odd and even fielf interlaced.

