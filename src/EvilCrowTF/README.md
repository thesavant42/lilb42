Originally from https://github.com/joelsernamoreno/EvilCrowRF-V2/tree/main/firmware/other/ECRFv2-flipper

Copy over the contents of the SDCARD fiolder onto an SD card.

I need to test if I can get away with setting this next bit in the Platformio.ini file or if I need to manually patch the library. I patched the library to have one less variable to eliminate while developing.

Edit AsyncTCP/src/AsyncTCP.h and change the following:

`#define CONFIG_ASYNC_TCP_USE_WDT 1` to `#define CONFIG_ASYNC_TCP_USE_WDT 0`


Spiffs will fail on first boot, unplug and replug the device. 

Spiffs will work now, but I believe a third booting is needed in order to bring the whole thing back up.

`ECRFv2` is the accesspoint, `123456789` is the pin. You can change it in source or by editing the config on the SD card.
