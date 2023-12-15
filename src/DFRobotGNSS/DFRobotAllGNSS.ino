 /*!
  * @file  getAllGNSS.ino
  * @brief read all gnss data
  * @copyright Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  * @license The MIT License (MIT)
  * @author ZhixinLiu(zhixin.liu@dfrobot.com)
  * @version V1.0
  * @date 2023-03-07
  * @url https://github.com/dfrobot/DFRobot_GNSS
  */

#include <Arduino.h>
#include <Wire.h>
#include <DFRobot_GNSS.h>

TwoWire I2CBackpack = TwoWire(0);

void callback(char *data ,uint8_t len)
{
  for(uint8_t i = 0; i < len; i++){
    Serial.print((char)data[i]);
  }
  delay(1);
}

#define I2C_COMMUNICATION  //use I2C for communication, but use the serial port for communication if the line of codes were masked
DFRobot_GNSS_I2C gnss(&I2CBackpack ,GNSS_DEVICE_ADDR);

void setup()
{
  Serial.begin(115200);
  I2CBackpack.begin(I2C_SDA, I2C_SCL, 400000);
  while(!gnss.begin()){
    Serial.println("NO Deivces !");
    delay(1000);
  }
  gnss.enablePower();      // Enable gnss power

/** Set GNSS to be used 
 *   eGPS              use gps
 *   eGLONASS          use glonass
 *   eGPS_GLONASS      use gps + glonass
 */
  gnss.setGnss(eGPS_BeiDou_GLONASS);

  gnss.setCallback(callback);
}

void loop()
{
  gnss.getAllGnss();
  delay(3000);
}