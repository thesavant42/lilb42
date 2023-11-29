#define DEST_FS_USES_SD

#define eepromsize 4096
#define samplesize 2000

#if defined(ESP8266)
    #define RECEIVE_ATTR ICACHE_RAM_ATTR
#elif defined(ESP32)
    #define RECEIVE_ATTR IRAM_ATTR
#else
    #define RECEIVE_ATTR
#endif

#define MIN_SAMPLE 30
#define ERROR_TOLERANZ 200
#define BUFFER_MAX_SIZE 2000
#define FORMAT_ON_FAIL true

#define FZ_SUB_MAX_SIZE 4096  // should be suficient
#define MAX_LINE_SIZE 4096
#define JSON_DOC_SIZE 4096

#define SERIAL_BAUDRATE 38400
#define DELAY_BETWEEN_RETRANSMISSIONS 2000

#define WEB_SERVER_PORT 80

/* I/O */
// SPI devices
#define SD_SCLK SDCARD_SCK_PIN
#define SD_MISO SDCARD_MISO_PIN
#define SD_MOSI SDCARD_MOSI_PIN
#define SD_SS   SDCARD_SS_PIN

#define CC1101_SCK  RF_SCK
#define CC1101_MISO RF_MISO
#define CC1101_MOSI RF_MOSI
#define CC1101_SS0  RF_CS
#define MOD0_GDO0 RF_GDO0
#define MOD0_GDO2 RF_GDO2

// Buttons and led
//#define LED LED
//#define BUTTON1 BUTTON1
//#define BUTTON2 BUTTON2

/* HTTP response codes */
#define HTTP_OK 200
#define HTTP_CREATED 201
#define HTTP_BAD_REQUEST 400
#define HTTP_UNAUTHORIZED 401
#define HTTP_FORBIDDEN 403
#define HTTP_NOT_FOUND 404
#define HTTP_NOT_IMPLEMENTED 501