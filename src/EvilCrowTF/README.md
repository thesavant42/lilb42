# Evilcrow-TF (Evil Crow: Taserface Edition)

Originally from https://github.com/joelsernamoreno/EvilCrowRF-V2/tree/main/firmware/other/ECRFv2-flipper

Replay Flipper Zero .sub files, and URH XML files from the Taserface Web interface.

## Installation

### Prepare the SD Card
Copy over the contents of the [SDCARD fiolder](https://github.com/joelsernamoreno/EvilCrowRF-V2/tree/main/firmware/other/ECRFv2-flipper/SD) onto an SD card.

### Patch AsyncTCP.h
I need to test if I can get away with setting this next bit in the Platformio.ini file or if I need to manually patch the library. I patched the library to have one less variable to eliminate while developing.

Edit AsyncTCP/src/AsyncTCP.h and change the following:

`#define CONFIG_ASYNC_TCP_USE_WDT 1` to `#define CONFIG_ASYNC_TCP_USE_WDT 0`


### SPIFFS

Spiffs will fail on first boot, unplug and replug the device. 

Spiffs will work now, but I believe a third booting is needed in order to bring the whole thing back up.

The app requires a SPIFFS filesystem to function. You will need to create a directory called "data" in the root of this PlatformIO project.
In the PlatformIO Sidebar, select Project Tasks -> Platform -> Builld Filesystem Image. This should very quickly complete without incident. When the device is ready to be flashed (no open serial monitors, no wifi connections), select Upload Filesystem Image.  Once the upload successfully completes, restart the device via the Reset button to load the new settings.

### SD

Copy the contents of this directory to the SD Card. This contains the HTML for the Web App, which will not function without these files. This is also where .SUB and URH XML files will live.

### Wifi 

`ECRFv2` is the accesspoint, 

`123456789` is the pin. 

You can change it in source or by editing the config on the SD card.
