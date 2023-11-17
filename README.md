 # LilB - Lilygo ePaper Mini Core 1.02 + CC1101 (and other) module

![twin Bees](./docs/IMG_0131.jpg)

Tiny ESP32 with ePaper Display, SDCard, and (perhaps most interestingly) a CC1101 Transciever / IR transciever backpacck. This project 
uses a fork of the Elechouse-CC1101 drivers, to account for the radio being on the secondary SPI bus of the epaper mini.

<h1 align = "center">🌟LilyGO Mini Epaper🌟</h1>

--------------------------------------
## 1️⃣Product

| Product(PinMap)              | SOC         | Flash | PSRAM     | Resolution | Size      | Driver       |
| ---------------------------- | ----------- | ----- | --------- | ---------- | --------- | ------------ |
| [Mini-E-Paper-ESP32 PICO][1] | ESP32-PICO  | 4MB   | x         | 128x80     | 1.02 Inch | GDGDEW0102T4 |
| [Mini-E-Paper-S3][2]         | ESP32-FN4R2 | 4MB   | 2MB(QSPI) | 128x80     | 1.02 Inch | GDGDEW0102T4 |

[1]: https://www.lilygo.cc/products/mini-e-paper-core?variant=42466407710901
[2]: https://www.lilygo.cc

<h2 align = "left">Quick start ⚡:</h2>

## 2️⃣ PlatformIO Quick Start (Recommended)

1. Install [Visual Studio Code](https://code.visualstudio.com/) and [Python](https://www.python.org/)
2. Search for the `PlatformIO` plugin in the `VisualStudioCode` extension and install it.
3. After the installation is complete, you need to restart `VisualStudioCode`
4. After restarting `VisualStudioCode`, select `File` in the upper left corner of `VisualStudioCode` -> `Open Folder` -> select the `LilyGO-Mini-Epaper` directory
5. Wait for the installation of third-party dependent libraries to complete
6. Click on the `platformio.ini` file, and in the `platformio` column
7. Select the board name you want to use in `default_envs` and uncomment it.
8. Uncomment one of the lines `src_dir = xxxx` to make sure only one line works
9. Click the (✔) symbol in the lower left corner to compile
10. Connect the board to the computer USB
11. Click (→) to upload firmware
12. Click (plug symbol) to monitor serial output
13. If it cannot be written, or the USB device keeps flashing, please check the **FAQ** below

## 3️⃣ Arduino IDE Quick Start

* It is recommended to use platformio without cumbersome steps

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Install [Arduino ESP32 2.0.5 or above](https://docs.espressif.com/projects/arduino-esp32/en/latest/) 
3. Copy all folders in [lib folder](./lib/)  to Arduino library folder (e.g. C:\Users\YourName\Documents\Arduino\libraries)
4. Open ArduinoIDE  ,`Tools` , Look at the picture to choose
   ![setting](images/ArduinoIDE.jpg)
5. `LilyGO-Mini-Epaper` -> `Any Examples`
6. Select `Port` (`Mini-E-Paper-ESP32 PICO`  requires an external U2T to discover the port)
7. Click `upload` , Wait for compilation and writing to complete
8. If it cannot be written, or the USB device keeps flashing, please check the **FAQ** below

# 6️⃣ FAQ

1. `Mini-E-Paper-S3` uses USB as the JTAG upload port. When printing serial port information on USB_CDC_ON_BOOT configuration needs to be turned on.
2. If the port cannot be found when uploading the program or the USB has been used for other functions, the port does not appear.
Please enter the upload mode manually.
   1. Connect the board via the USB cable (`Mini-E-Paper-ESP32 PICO`  requires an external U2T to discover the port)
   2. Press and hold the BOOT button , While still pressing the BOOT button, press RST
      ![BOOT](./images/download.jpg)
   3. Release the RST
   4. Release the BOOT button
   5. Upload sketch
3. If the above is invalid, burn the [binary file](./firmware/README.MD)  to check whether the hardware is normal

## Examples

### Working:

[GxEPD_TF_Card_Example](./src/GxEPD_TF_Card_Example/) -- Works as expected.
[cc1101-tool-esp32](./src/cc1101-tool-esp32/)  -- This appears to work as expected. 
[lilb42](./src/lilb42/) -- "Works", epd is updated with logo, not sure if the tx is working as expected

### Might be working:

[Esp32-CC1101-Standalone](./src/Esp32-CC1101-Standalone/) - "works" I think, not tested conclusively

### Not working yet:

[Esp32-SubGhz](./src/Esp32-SubGhz/) - Blocked, depends upoin SDFat which does not seem to want to play nicely with the TF Card of the EPaper Mini Core 1.02. Most likely because the TF Card SPI is non-standard for ESP32:

Speaking of, here is pin mapping:

## Pin Mappings
### TF Card

```
MOSI 15
SCLK 14
CS   13
MISO 2
```

### EPD

```
MOSI	21
MISO	-1
CLK 	22
CS      5
RST 	4
BUSY	34
```

### Power_Enable

"In a new program, **this pin needs to output "HIGH"**, so that the product starts to run the program function"

```
IO27
```

### 3 Way Rocker Switch 

(Facing EPD Display)

```
Left	GPIO39
Right	GPIO36
Down	GPIO0   // (Also boot button)
```

### Battery Detection:

```
IO35
```

![Official Pinmap Image](https://www.lilygo.cc/cdn/shop/products/MINI1.02CORE.jpg)

## CC1101 Backpack

### CC101 SPI

```
CC1101_SCLK	26
CC1101_MISO	38
CC1101_MOSI	23
CC1101_CS 	25	
CC1101_GD0 	32
CC1101_GD2 	37
```

### i2c

CC1101 StemmaQT i2C Port (3v)

```
SDA 	IO21
SCSL 	IO22
```

### IR???

There are also a pair of IR Receiver and Emitter LEDs, which can be seen referenced on the [schematic](.docs/schematic/Extension-CC1101.pdf).

```
#define IR_TX_PIN       12
#define IR_RECEIVE_PIN  18
```

The official CC1101 [RX](https://github.com/Xinyuan-LilyGO/LilyGO-Mini-Epaper/blob/main/examples/Extension/CC1101_RX/CC1101_RX.ino) and [TX](https://github.com/Xinyuan-LilyGO/LilyGO-Mini-Epaper/blob/main/examples/Extension/CC1101_TX/CC1101_TX.ino) examples have a subroutine for IR, but do they actually work?

```
#ifdef LILYGO_MINI_EPAPER_ESP32
#define CC1101_SCLK     26
#define CC1101_MISO     38
#define CC1101_MOSI     23
#define CC1101_CS       25
#define CC1101_GDO0     32
#define CC1101_GDO2     37
#define IR_TX_PIN       12
#define IR_RECEIVE_PIN  18
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

```
MISO 	IO38
MOSI 	IO23
IRQ 	IO37
SCLK 	IO26
CS 		IO25
CE 		IO12
```

### i2C

```
SDA 	IO18
SCL 	IO33
```

![Official Image of NRF24L01 Backpack Pinmap](https://ae01.alicdn.com/kf/Sdfea02bfab7f414fb55f6797c0865b22Q.jpg_640x640q90.jpg)

## More Info

### Links

[Lilygo Repo](https://github.com/Xinyuan-LilyGO/LilyGO-Mini-Epaper) -- The Official Lilygo Repo for the Epaper Mini

[Esp32-SubGhz](https://github.com/simondankelmann/Esp32-SubGhz) -- Suite that uses esp32 + cc1101 to capture and replay signals. Supports reading Flipper Zero .SUB SubGhz fies, if we can get the SDCard to pay nice

[cc1101-tool](https://github.com/mcore1976/cc1101-tool) -- High level command line access to the CC1101 Radio API. 

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
