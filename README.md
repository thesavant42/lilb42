# LilB - Lilygo ePaper Mini Core 1.02 + CC1101 (and other) module

This is a Standalone-Version with 4 Push-Buttons and 2 Led's to indicate RX and TX Operations. It is for Example possible to define 4 Signals in the Arduino Code that are transmitted when a button is pressed. 
Uses a fork of the Elechouse-CC1101 drivers, to account for the radio being on the secondary SPI bus of the epaper mini.

## Examples
### Working:
GxEPD_TF_Card_Example - Works as expected.
cc1101-tool-esp32  - This appears to work as expected. 
lilb42 - "Works", epd is updated with logo, not sure if the tx is working as expected

### Might be working:
Esp32-CC1101-Standalone - "works" I think, not tested conclusively

### Not working yet:
Esp32-SubGhz - Blocked, depends upoin SDFat which does not seem to want to play nicely with the TF Card of the EPaper Mini Core 1.02. Most likely because the TF Card SPI is non-standard for ESP32:

Speaking of, here is pin mapping:

## Pin Mappings
### TF Card

MOSI 15
SCLK 14
CS   13
MISO 2

### EPD

MOSI	21
MISO	-1
CLK 	22
CS 		5
RST 	4
BUSY	34

### 3 Way Rocker Switch 
(Facing EPD Display)
Left	GPIO39
Right	GPIO36
Down	GPIO0 (Also boot button)

![Official Pinmap Image](https://www.lilygo.cc/cdn/shop/products/MINI1.02CORE.jpg)

## CC1101 Backpack
### CC101 SPI
CC1101_SCLK	26
CC1101_MISO	38
CC1101_MOSI	23
CC1101_CS 	25	
CC1101_GD0 	32
CC1101_GD2 	37

### i2c
CC1101 StemmaQT i2C Port (3v)
SDA 	IO21
SCSL 	IO22

### IR???
The official CC1101 [RX] (https://github.com/Xinyuan-LilyGO/LilyGO-Mini-Epaper/blob/main/examples/Extension/CC1101_RX/CC1101_RX.ino) and [TX] (https://github.com/Xinyuan-LilyGO/LilyGO-Mini-Epaper/blob/main/examples/Extension/CC1101_TX/CC1101_TX.ino) examples have a subroutine for IR, does this actually work?

```
#ifdef LILYGO_MINI_EPAPER_ESP32
#define CC1101_SCLK     26
#define CC1101_MISO     38
#define CC1101_MOSI     23
#define CC1101_CS       25
#define CC1101_GDO0     32
#define CC1101_GDO2     37
#define IR_TX_PIN        12
#define IR_RECEIVE_PIN   18
#else
#define CC1101_SCLK     8
#define CC1101_MISO     6
#define CC1101_MOSI     17
#define CC1101_CS       7
#define CC1101_GDO0     5
#define CC1101_GDO2     16
#define IR_TX_PIN        21
#define IR_RECEIVE_PIN   18
#endif
```


![Official CC1101 Backpack Pinmap](https://ae01.alicdn.com/kf/S6641485d4f53444e9c491b434508d01cm.jpg_640x640q90.jpg)

## NRF24L01 Backack

### SPI (nrf24l01)

MISO 	IO38
MOSI 	IO23
IRQ 	IO37
SCLK 	IO26
CS 		IO25
CE 		IO12

### i2C
SDA 	IO18
SCL 	IO33

![Official Image of NRF24L01 Backpack Pinmap](https://ae01.alicdn.com/kf/Sdfea02bfab7f414fb55f6797c0865b22Q.jpg_640x640q90.jpg)

## More Info

### Links

[Lilygo Repo] (https://github.com/Xinyuan-LilyGO/LilyGO-Mini-Epaper) -- The Official Lilygo Repo for the Epaper Mini

[Esp32-SubGhz] (https://github.com/simondankelmann/Esp32-SubGhz) -- Suite that uses esp32 + cc1101 to capture and replay signals. Supports reading Flipper Zero .SUB SubGhz fies, if we can get the SDCard to pay nice

[cc1101-tool] (https://github.com/mcore1976/cc1101-tool) -- High level command line access to the CC1101 Radio API. 

## Flipper Zero IR and SubGhz Files
May as well shoot for feature-parity (at a 10th of the cost) . The CC1101 backpack (seems to? Maybe?) have IR, would be nice to port these dBs over. The S3 version of the ePaper Mini Coure *should* support HID/BadUSB 

### Sub-Ghz, Remotes, IR, Files, Databases & Dumps etc..

[UberGuidoZ Playground - Large collection of files - Github](https://github.com/UberGuidoZ/Flipper) -- Large collection of files, documentation, and dumps of all kinds.

[Awesome Flipper Zero](https://github.com/djsime1/awesome-flipperzero) -- A collection of Awesome resources for the Flipper Zero device.

[FlipperZero-TouchTunes](https://github.com/jimilinuxguy/flipperzero-touchtunes) -- TouchTunes remote dump

[Universal Intercom Keys](https://github.com/GlUTEN-BASH/Flipper-Starnew) -- Universal Intercom Keys

[FlipperZero-Goodies](https://github.com/wetox-team/flipperzero-goodies) -- Intercom keys, scripts.

[Flipper-IRDB](https://github.com/Lucaslhm/Flipper-IRDB) -- IR dumps

[XBox IR Controller](https://github.com/gebeto/flipper-xbox-controller) -- Control XBox One via IR

[PAGGER](https://meoker.github.io/pagger/) -- A collection of Sub-GHz files generators compatible with the Flipper Zero to handle restaurants/kiosks paging systems.
