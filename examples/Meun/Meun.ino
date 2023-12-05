/*
Weather_clock Test example

 */
// According to the board, cancel the corresponding macro definition
// https://www.lilygo.cc/products/mini-e-paper-core , esp32picod4
// #define LILYGO_MINI_EPAPER_ESP32
// esp32s3-fn4r2
// #define LILYGO_MINI_EPAPER_ESP32S3

#if !defined(LILYGO_MINI_EPAPER_ESP32S3)  && !defined(LILYGO_MINI_EPAPER_ESP32)
// 请在草图上方选择对应的目标板子名称,将它取消注释.
#error "Please select the corresponding target board name above the sketch and uncomment it."
#endif
#include <Arduino.h>
#include <GxEPD.h>
#include <boards.h>
#include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w

#include GxEPD_BitmapExamples
#include <U8g2_for_Adafruit_GFX.h>
#include <GxIO/GxIO.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>

#include <WiFi.h>
#include <Wire.h>

#define TIME_ZONE  8
#define TIME_WEATHER_SYNC //Reset to enable time weather calibration. You can comment it out

#include <FunctionalInterrupt.h>
#include "battery_index.h"

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#include <stdio.h>

#include <FS.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <time.h>
#include <pcf8563.h>

const char *ssid = "YOURWIFISSID";
const char *password = "YOURASSWORD";

// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "APIKEY";
// Replace with your country code and city
String city = "Malibu";
String countryCode = "US";

String jsonBuffer;
uint32_t    last = 0;
uint32_t    update_flag = 0;
uint32_t    wifi_flag = 0;

const char *ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * (TIME_ZONE - 4);
const int   daylightOffset_sec = 3600 * (TIME_ZONE - 4);

GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
struct tm timeinfo;
String current_weather;
PCF8563_Class rtc;
int calibrationData[3][2];
bool changed = false;
char myArray[3];

int8_t rslt;
/* Variable to get the step counter output */
uint32_t step_out = 0;
/* Variable to get the interrupt status  */
uint16_t int_status = 0;
uint16_t counter_IRQ = 0;

/* __  __ ______ _   _ _    _
 |  \/  |  ____| \ | | |  | |
 | \  / | |__  |  \| | |  | |
 | |\/| |  __| | . ` | |  | |
 | |  | | |____| |\  | |__| |
 |_|  |_|______|_| \_|\____/
*/
#define ICON_WIDTH (51)
#define ICON_HEIGHT (51)
#define ICON_GAP (4)
#define ICON_BGAP (16)
#define ICON_Y (72+ ICON_GAP)

struct menu_entry_type {
    const uint8_t *font;
    uint16_t icon;
    const char *name;
};

struct menu_state {
    int16_t menu_start;       /* in pixel */
    int16_t frame_position;       /* in pixel */
    uint8_t position;         /* position, array index */
};


struct menu_entry_type menu_entry_list[] = {
    { u8g2_font_open_iconic_all_6x_t, 123, "Integration"},
    { u8g2_font_open_iconic_all_6x_t, 93, "Clock "},
    //{ u8g2_font_open_iconic_all_6x_t, 225, "Music"},
    { u8g2_font_open_iconic_all_6x_t, 259, "Weather"},
    //{ u8g2_font_open_iconic_all_6x_t, 136, "Compass"},
    { u8g2_font_open_iconic_all_6x_t, 94, "Bluetooth"},
    { u8g2_font_open_iconic_all_6x_t, 281, "WIFI"},
    { NULL, 0, NULL }
};


struct menu_state current_state = { ICON_BGAP, ICON_BGAP, 0 };
struct menu_state destination_state = { ICON_BGAP, ICON_BGAP, 0 };

bool first = true;
bool meun_update_flag = false;

/*
  ____  _    _ _______ _______ ____  _   _
 |  _ \| |  | |__   __|__   __/ __ \| \ | |
 | |_) | |  | |  | |     | | | |  | |  \| |
 |  _ <| |  | |  | |     | | | |  | | . ` |
 | |_) | |__| |  | |     | | | |__| | |\  |
 |____/ \____/   |_|     |_|  \____/|_| \_|
 */
bool button1_flag = 0;
bool button2_flag = 0;
bool button3_flag = 0;
bool button3_long_flag = 0;

#include <AceButton.h>
using namespace ace_button;

// Both buttons automatically use the default System ButtonConfig. The
// alternative is to call the AceButton::init() method in setup() below.
AceButton button1(BUTTON_2); // Swapping to match rotated screen
AceButton button2(BUTTON_1); //
AceButton button3((uint8_t)(BUTTON_3));

// Forward reference to prevent Arduino compiler becoming confused.
void handleEvent(AceButton *, uint8_t, uint8_t);

uint16_t writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
uint16_t readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
void LilyGo_logo();

bool printLocalTime();
void display_time();
void display_Battery();
void get_weather();
String httpGETRequest(const char *serverName);
void display_weather();
void display_wifi();
void EnterSleep();
void LilyGo_logo();


void draw(struct menu_state *state);
void to_right(struct menu_state *state);
void to_left(struct menu_state *state);
uint8_t towards_int16(int16_t *current, int16_t dest);
uint8_t towards(struct menu_state *current, struct menu_state *destination);
uint8_t meun_loop();

uint8_t clock_weather_loop();
uint8_t clock_loop();
uint8_t weather_loop();
uint8_t Compass_loop();
uint8_t Bluetooth_loop();
uint8_t WIFI_loop();

void setup(void)
{

    Serial.begin(115200);
    Serial.println("setup");

    pinMode(EPD_POWER_ENABLE, OUTPUT);
    digitalWrite(EPD_POWER_ENABLE, HIGH);

    // Buttons use the built-in pull up register.
    pinMode(BUTTON_1, INPUT_PULLUP);
    pinMode(BUTTON_2, INPUT_PULLUP);
    pinMode(BUTTON_3, INPUT_PULLUP);

    // Configure the ButtonConfig with the event handler, and enable all higher
    // level events.
    ButtonConfig *buttonConfig = ButtonConfig::getSystemButtonConfig();
    buttonConfig->setEventHandler(handleEvent);
    buttonConfig->setFeature(ButtonConfig::kFeatureClick);
    buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
    buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
    buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);


    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
    display.init(); // enable diagnostic output on Serial
    u8g2Fonts.begin(display);
    display.setTextColor(GxEPD_BLACK);
    u8g2Fonts.setFontMode(1);                           // use u8g2 transparent mode (this is default)
    u8g2Fonts.setFontDirection(1);                      // left to right (this is default)
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);          // apply Adafruit GFX color
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);          // apply Adafruit GFX color

    Wire.begin(IIC_SDA, IIC_SCL);
    rtc.begin();

#ifdef TIME_WEATHER_SYNC
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" CONNECTED");

    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    while (!printLocalTime()); //get time

    get_weather();


    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
#else
#endif

    LilyGo_logo();
    Serial.println("setup done");

}

void loop()
{
    uint8_t val = meun_loop();
    Serial.printf("Select : %d\n", val);
    switch (val) {
    case 0:
        clock_weather_loop();
        break;
    case 1:
        clock_loop();
        break;
    case 2:
        weather_loop();
        break;
    case 3:
        Compass_loop();
        break;
    case 4:
        Bluetooth_loop();
        break;
    case 5:
        WIFI_loop();
        break;
    default:
        break;
    }



}

/*
  __  __          _____ _   _ _      ____   ____  _____
 |  \/  |   /\   |_   _| \ | | |    / __ \ / __ \|  __ \
 | \  / |  /  \    | | |  \| | |   | |  | | |  | | |__) |
 | |\/| | / /\ \   | | | . ` | |   | |  | | |  | |  ___/
 | |  | |/ ____ \ _| |_| |\  | |___| |__| | |__| | |
 |_|  |_/_/    \_\_____|_| \_|______\____/ \____/|_|

*/
uint8_t clock_weather_loop()
{

    display.setRotation(2);
    display.fillScreen(GxEPD_WHITE);
    display_time();
    display_Battery();
    display_weather();

    display.update();
    delay(100);
    for (;;) {
        /* code */


        if (millis() - last > 10000) {
            display_time();
            display_Battery();
            display_weather();
            display_wifi();
            if (update_flag == 6) {
                display.update();
                update_flag = 0;
            }
            display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);//display.update();////display.update();
            update_flag++;
            last = millis();
        }



        button1.check();
        button2.check();
        button3.check();


        if (button3_flag)  {
            wifi_status();
            button3_flag = 0;
        }
        if (button1_flag)  {
            button1_flag = 0 ;
            return 0;
        }
        if (button2_flag)  {
            button2_flag = 0;
            return 0;
        }
        if (button3_long_flag)  {
            button3_long_flag = 0;
            EnterSleep();
        }


    }
}

uint8_t clock_loop()
{
    display.setRotation(2);
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 50);
    display.println("clock_loop");
    display.println("You can customize the functionality ");
    display.update();

    for (;;) {

        button1.check();
        button2.check();
        button3.check();

        if (button3_flag)  {
            button3_flag = 0;
        }
        if (button1_flag)  {
            button1_flag = 0 ;
            return 0;
        }
        if (button2_flag)  {
            button2_flag = 0;
            return 0;
        }
        if (button3_long_flag)  {
            button3_long_flag = 0;
            EnterSleep();
        }

    }
}

uint8_t weather_loop()
{
    display.setRotation(2);
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 50);
    display.println("weather_loop");
    display.println("You can customize the functionality ");
    display.update();

    for (;;) {

        button1.check();
        button2.check();
        button3.check();

        if (button3_flag)  {
            button3_flag = 0;
        }
        if (button1_flag)  {
            button1_flag = 0 ;
            return 0;
        }
        if (button2_flag)  {
            button2_flag = 0;
            return 0;
        }
        if (button3_long_flag)  {
            button3_long_flag = 0;
            EnterSleep();
        }

    }
}

uint8_t Compass_loop()
{
    display.setRotation(2);
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 50);
    display.println("Compass_loop");
    display.println("You can customize the functionality ");
    display.update();

    for (;;) {

        button1.check();
        button2.check();
        button3.check();

        if (button3_flag)  {
            button3_flag = 0;
        }
        if (button1_flag)  {
            button1_flag = 0 ;
            return 0;
        }
        if (button2_flag)  {
            button2_flag = 0;
            return 0;
        }
        if (button3_long_flag)  {
            button3_long_flag = 0;
            EnterSleep();
        }

    }
}

uint8_t Bluetooth_loop()
{
    display.setRotation(2);
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 50);
    display.println("Bluetooth_loop");
    display.println("You can customize the functionality ");
    display.update();

    for (;;) {

        button1.check();
        button2.check();
        button3.check();

        if (button3_flag)  {
            button3_flag = 0;
        }
        if (button1_flag)  {
            button1_flag = 0 ;
            return 0;
        }
        if (button2_flag)  {
            button2_flag = 0;
            return 0;
        }
        if (button3_long_flag)  {
            button3_long_flag = 0;
            EnterSleep();
        }

    }
}

uint8_t WIFI_loop()
{
    display.setRotation(2);
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 50);
    display.println("WIFI_loop");
    display.println("You can customize the functionality ");
    display.update();

    for (;;) {

        button1.check();
        button2.check();
        button3.check();

        if (button3_flag)  {
            button3_flag = 0;
        }
        if (button1_flag)  {
            button1_flag = 0 ;
            return 0;
        }
        if (button2_flag)  {
            button2_flag = 0;
            return 0;
        }
        if (button3_long_flag)  {
            button3_long_flag = 0;
            EnterSleep();
        }

    }
}

bool check_button(uint8_t pin)
{
    if (digitalRead(pin) == 0)   {
        delay(200);
        //Wait for release
        while (!digitalRead(pin));
        return true;
    } else return false;

}

/*
 __          ________       _______ _    _ ______ _____
 \ \        / /  ____|   /\|__   __| |  | |  ____|  __ \
  \ \  /\  / /| |__     /  \  | |  | |__| | |__  | |__) |
   \ \/  \/ / |  __|   / /\ \ | |  |  __  |  __| |  _  /
    \  /\  /  | |____ / ____ \| |  | |  | | |____| | \ \
     \/  \/   |______/_/    \_\_|  |_|  |_|______|_|  \_\
*/

void display_weather()
{
    get_weather();
    display.setRotation(2);
    u8g2Fonts.setFont(u8g2_font_open_iconic_weather_2x_t ); //  weather iconic, select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    //display.fillScreen(GxEPD_WHITE);
    u8g2Fonts.setCursor(10, 5);
    if   (current_weather == "Clear")  u8g2Fonts.print("E");//clear
    else if (current_weather == "Clouds")u8g2Fonts.print("A");//Clouds
    else if (current_weather == "Rain")  u8g2Fonts.print("C"); //Rain
    u8g2Fonts.setFont(u8g2_font_helvR10_te  );
    u8g2Fonts.setCursor(10, 25);
    u8g2Fonts.print(current_weather);
}

void get_weather()
{
    if (WiFi.status() == WL_CONNECTED) {
        String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;

        jsonBuffer = httpGETRequest(serverPath.c_str());
        Serial.println(jsonBuffer);
        JSONVar myObject = JSON.parse(jsonBuffer);

        while (JSON.typeof(myObject) == "undefined");
        // JSON.typeof(jsonVar) can be used to get the type of the var
        if (JSON.typeof(myObject) == "undefined") {
            Serial.println("Parsing input failed!");
            return;
        }

        Serial.print("JSON object = ");
        Serial.println(myObject);
        /*Serial.print("Temperature: ");
        Serial.println(myObject["main"]["temp"]);
        Serial.print("Pressure: ");
        Serial.println(myObject["main"]["pressure"]);
        Serial.print("Humidity: ");
        Serial.println(myObject["main"]["humidity"]);
        Serial.print("Wind Speed: ");
        Serial.println(myObject["wind"]["speed"]);
        Serial.print("weather main: ");
        Serial.println(myObject["weather"][0]["main"]);
        Serial.print("weather icon: ");
        Serial.println(myObject["weather"][0]["icon"]);
        */
        current_weather = (const char*)myObject["weather"][0]["main"];

    } else {
        return;
        //Serial.println("WiFi Disconnected");
    }



}

String httpGETRequest(const char *serverName)
{
    HTTPClient http;

    // Your IP address with path or Domain name with URL path
    http.begin(serverName);

    // Send HTTP POST request
    int httpResponseCode = http.GET();

    String payload = "{}";

    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
    } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    return payload;
}


//wifi_status
void wifi_status(void)
{
    if (wifi_flag == 0) {

        u8g2Fonts.setFont(u8g2_font_open_iconic_embedded_2x_t); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
        u8g2Fonts.setCursor(110, 5);
        u8g2Fonts.print("...");
        Serial.printf("Connecting to %s ", ssid);
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println(" CONNECTED");

        display.setRotation(2);
        u8g2Fonts.setFont(u8g2_font_helvR10_te); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
        u8g2Fonts.setCursor(110, 5);
        u8g2Fonts.print("P");
        display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
        //init and get the time
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        printLocalTime();

        //get weather date
        get_weather();
        wifi_flag = 1;
    } else {
        //disconnect WiFi as it's no longer needed
        WiFi.disconnect();
        u8g2Fonts.setFont(u8g2_font_open_iconic_other_2x_t); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
        u8g2Fonts.setCursor(110, 5);
        u8g2Fonts.print("F");
        display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
        wifi_flag = 0;
    }


}

void display_wifi()
{
    if (wifi_flag == 1) {//show  wifi icon
        display.setRotation(2);
        u8g2Fonts.setFont(u8g2_font_open_iconic_embedded_2x_t); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
        u8g2Fonts.setCursor(110, 5);
        u8g2Fonts.print("P");
    } else { //show  wifi icon
        display.setRotation(2);
        u8g2Fonts.setFont(u8g2_font_open_iconic_other_2x_t); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
        u8g2Fonts.setCursor(110, 5);
        u8g2Fonts.print("F");

    }

}

void display_Battery()
{

    int sensorValue = analogRead(ADC_PIN);
    float voltage = sensorValue * (3.3 / 4096);
    delay(100);
    voltage = (voltage * 2) - 3.0;
    int ADC_Int = (int)(voltage / 0.083);

    if (ADC_Int > 12) ADC_Int = 12;
    else if (ADC_Int < 0) ADC_Int = 0;
    // print out the value you read:
    //Serial.println(sensorValue);
    //Serial.println(voltage);
    display.setRotation(2);
    display.drawExampleBitmap(BMP[ADC_Int], 65, 0, 16, 8, GxEPD_BLACK);



}

/*
  _______ _____ __  __ ______
 |__   __|_   _|  \/  |  ____|
    | |    | | | \  / | |__
    | |    | | | |\/| |  __|
    | |   _| |_| |  | | |____
    |_|  |_____|_|  |_|______|
 */
void display_time()
{
    //printLocalTime();
    Serial.println(rtc.formatDateTime(PCF_TIMEFORMAT_HM));
    Serial.println(rtc.formatDateTime(PCF_TIMEFORMAT_YYYY_MM_DD));

    display.setRotation(2);
    display.fillScreen(GxEPD_WHITE);
    u8g2Fonts.setFont(u8g2_font_fub20_tf );
    display.fillScreen(GxEPD_WHITE);
    u8g2Fonts.setCursor(80, 5);
    // u8g2Fonts.println(&timeinfo, "%H:%M");
    u8g2Fonts.println(rtc.formatDateTime(PCF_TIMEFORMAT_HM));

    u8g2Fonts.setCursor(50 + 10, 8);
    u8g2Fonts.setFont(u8g2_font_timB12_tr );
    u8g2Fonts.println(rtc.formatDateTime(PCF_TIMEFORMAT_YYYY_MM_DD));
    /*
    u8g2Fonts.print(timeinfo.tm_year + 1900);
    u8g2Fonts.print("/");
    u8g2Fonts.print(timeinfo.tm_mon + 1);
    u8g2Fonts.print("/");
    u8g2Fonts.println(timeinfo.tm_mday);*/

    u8g2Fonts.setCursor(35 + 10, 30);
    u8g2Fonts.setFont(u8g2_font_helvR10_te);
    u8g2Fonts.println(&timeinfo, "%a");

    display.setRotation(2);
    display.drawExampleBitmap(Walk_icon, 20, 85, 16, 18, GxEPD_BLACK);

    //Output the number
    display.setRotation(2);
    u8g2Fonts.setCursor(28, 35);
    u8g2Fonts.println(step_out);

    // display "↑"
    u8g2Fonts.setFont(u8g2_font_cu12_t_symbols);
    u8g2Fonts.drawGlyph(103, 48, 8593);

}

bool printLocalTime()
{
    int restart_flag = 0;
    int last = millis();
    int while_flag = 0;

    while (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        //WIFI iconic
        display.setRotation(2);
        u8g2Fonts.setFont(u8g2_font_helvR10_te); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
        display.fillScreen(GxEPD_WHITE);
        u8g2Fonts.setCursor(90, 5);
        u8g2Fonts.print("updating...");
        display.update();
        //return false;
        if (millis() - last > 10000) {
            ESP.restart();
        }
    }

    rtc.getDayOfWeek(timeinfo.tm_mday,  timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
    rtc.setDateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    return true;


}


/*
    ____  __  __  _____
  / __ \|  \/  |/ ____|
 | |  | | \  / | |
 | |  | | |\/| | |
 | |__| | |  | | |____
  \___\_\_|  |_|\_____|
*/
const char bearings[16][3] =  {
    {' ', ' ', 'W'},
    {'W', 'S', 'W'},
    {' ', 'S', 'W'},
    {'S', 'S', 'W'},
    {' ', ' ', 'S'},
    {'S', 'S', 'E'},
    {' ', 'S', 'E'},
    {'E', 'S', 'E'},
    {' ', ' ', 'E'},
    {'E', 'N', 'E'},
    {' ', 'N', 'E'},
    {'N', 'N', 'E'},
    {' ', ' ', 'N'},
    {'N', 'N', 'W'},
    {' ', 'N', 'W'},
    {'W', 'N', 'W'},
};

// Low-level I2C Communication
// Provided to BMA423_Library communication interface
//
uint16_t readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)address, (uint8_t)len);
    uint8_t i = 0;
    while (Wire.available()) {
        data[i++] = Wire.read();
    }
    return 0; //Pass
}

uint16_t writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(data, len);
    return (0 !=  Wire.endTransmission());
}



/* __  __ ______ _   _ _    _
 |  \/  |  ____| \ | | |  | |
 | \  / | |__  |  \| | |  | |
 | |\/| |  __| | . ` | |  | |
 | |  | | |____| |\  | |__| |
 |_|  |_|______|_| \_|\____/
*/
uint8_t meun_loop()
{
    first = true;
    display.setTextColor(GxEPD_BLACK);
    u8g2Fonts.setFontMode(0);                           // use u8g2 transparent mode (this is default)
    u8g2Fonts.setFontDirection(1);                      // left to right (this is default)
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);          // apply Adafruit GFX color
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);          // apply Adafruit GFX color
    display.setRotation(2);
    Serial.println("meun_loop");

    button3_flag = 0;
    button1_flag = 0;
    button2_flag = 0;
    for (;;) {

        button1.check();
        button2.check();
        button3.check();

        display.fillScreen(GxEPD_WHITE);
        draw(&destination_state);
        u8g2Fonts.setFont(u8g2_font_helvB10_tr);
        u8g2Fonts.setCursor(8, 0);
        u8g2Fonts.print(menu_entry_list[destination_state.position].name);
        if (first || meun_update_flag) {
            display.update();
            first = false;
            meun_update_flag = false;
        }

        if (button1_flag  == 1 ) {
            Serial.println("button1_flag");
            to_left(&destination_state);
            draw(&current_state);
            meun_update_flag = true;
            button1_flag  = 0;
        }

        if ( button2_flag == 1 ) {
            Serial.println("button2_flag");
            to_right(&destination_state);
            draw(&current_state);
            meun_update_flag = true;
            button2_flag  = 0;
        }

        if ( button3_flag == 1 ) {
            Serial.println(destination_state.position);
            button3_flag  = 0;
            return destination_state.position;
        }

        if ( button3_long_flag == 1 ) {
            button3_long_flag  = 0;
            EnterSleep();
        }

    }


}

void draw(struct menu_state *state)
{
    int16_t x;
    uint8_t i;
    x = state->menu_start;

    i = 0;
    while ( menu_entry_list[i].icon > 0 ) {

        if (x >= (-ICON_WIDTH) && x < GxGDGDEW0102T4_HEIGHT  ) {

            u8g2Fonts.setFont(menu_entry_list[i].font);
            u8g2Fonts.drawGlyph(ICON_Y - 50, x - 12, menu_entry_list[i].icon);

        }

        i++;
        x += ICON_WIDTH + ICON_GAP;
    }
    display.drawRect(ICON_Y - ICON_HEIGHT - 1, state->frame_position - 1 - 13, ICON_WIDTH + 2, ICON_WIDTH + 2, GxEPD_BLACK);
    display.drawRect(ICON_Y - ICON_HEIGHT - 2, state->frame_position - 2 - 13, ICON_WIDTH + 4, ICON_WIDTH + 4, GxEPD_BLACK);
    display.drawRect(ICON_Y - ICON_HEIGHT - 3, state->frame_position - 3 - 13, ICON_WIDTH + 6, ICON_WIDTH + 6, GxEPD_BLACK);

}

void to_right(struct menu_state *state)
{

    if ( menu_entry_list[state->position + 1].font != NULL ) {

        if ( (int16_t)state->frame_position + 2 * (int16_t)ICON_WIDTH + (int16_t)ICON_BGAP < (int16_t)GxGDGDEW0102T4_HEIGHT ) {
            state->position++;
            state->frame_position += ICON_WIDTH + (int16_t)ICON_GAP;
        } else {
            state->position++;
            state->frame_position = (int16_t)GxGDGDEW0102T4_HEIGHT - (int16_t)ICON_WIDTH - (int16_t)ICON_BGAP;
            state->menu_start = state->frame_position - state->position * ((int16_t)ICON_WIDTH + (int16_t)ICON_GAP);
            delay(50);

        }
    }
}

void to_left(struct menu_state *state)
{
    if ( state->position > 0 ) {
        if ( (int16_t)state->frame_position >= (int16_t)ICON_BGAP + (int16_t)ICON_WIDTH + (int16_t)ICON_GAP ) {
            Serial.println("to_left1");
            state->position--;
            state->frame_position -= ICON_WIDTH + (int16_t)ICON_GAP;
        } else {
            Serial.println("to_left2");
            state->position--;
            state->frame_position = ICON_BGAP;
            state->menu_start = state->frame_position - state->position * ((int16_t)ICON_WIDTH + (int16_t)ICON_GAP);

        }
    }
}

uint8_t towards_int16(int16_t *current, int16_t dest)
{
    if ( *current < dest ) {
        (*current)++;
        return 1;
    } else if ( *current > dest ) {
        (*current)--;
        return 1;
    }
    return 0;
}

uint8_t towards(struct menu_state *current, struct menu_state *destination)
{
    uint8_t r = 0;
    r |= towards_int16( &(current->frame_position), destination->frame_position);
    r |= towards_int16( &(current->frame_position), destination->frame_position);
    r |= towards_int16( &(current->menu_start), destination->menu_start);
    r |= towards_int16( &(current->menu_start), destination->menu_start);
    return r;
}

void EnterSleep()
{
    display.setRotation(2);
    delay(1000);
    
    Serial.println("EnterSleep");
    display.fillScreen(GxEPD_WHITE);
    u8g2Fonts.setFont(u8g2_font_open_iconic_all_4x_t);
    u8g2Fonts.drawGlyph(65, 30, 235);
    //display.print("EnterSleep");
    display.update();
    delay(2000);
    esp_sleep_enable_ext1_wakeup(((uint64_t)(((uint64_t)1) << BUTTON_1)), ESP_EXT1_WAKEUP_ALL_LOW);
    esp_deep_sleep_start();
    /*Turn on power control*/
}

void LilyGo_logo(void)
{
    display.setRotation(2);
    display.fillScreen(GxEPD_WHITE);
    display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
    display.update();
    display.fillScreen(GxEPD_WHITE);

}

// The event handler for both buttons.
void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState)
{

    // Print out a message for all events, for both buttons.
    Serial.print(F("handleEvent(): pin: "));
    Serial.print(button->getPin());
    Serial.print(F("; eventType: "));
    Serial.print(eventType);
    Serial.print(F("; buttonState: "));
    Serial.println(buttonState);


    switch (eventType) {

    case AceButton::kEventReleased:
        if (button->getPin() == BUTTON_1) {
            // Serial.println(F("BUTTON_1 "));
            button1_flag = 1;
        }
        if (button->getPin() == BUTTON_2) {
            // Serial.println(F("BUTTON_2 "));
            button2_flag = 1;
        }
        if (button->getPin() == BUTTON_3) {
            //   Serial.println(F("BUTTON_3 "));
            button3_flag = 1;
        }
        break;
    case AceButton::kEventLongPressed:
        if (button->getPin() == BUTTON_3) {
            //  Serial.println(F("BUTTON_1 EventLongPressed!"));
            button3_long_flag = 1;
        }
        break;
    }
}
