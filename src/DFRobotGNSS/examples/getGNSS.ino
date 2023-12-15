 /*!
  * @file  getGNSS.ino
  * @brief Get gnss simple data
  * @copyright Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  * @license The MIT License (MIT)
  * @author ZhixinLiu(zhixin.liu@dfrobot.com)
  * @version V1.0
  * @date 2023-03-07
  * @url https://github.com/dfrobot/DFRobot_GNSS
  */
#include <Arduino.h>
#include <Wire.h>
#include "DFRobot_GNSS.h"

#define I2C_COMMUNICATION  //use I2C for communication, but use the serial port for communication if the line of codes were masked

TwoWire I2CBackpack = TwoWire(0); // JB This is how we need to invoke i2c on the backpack 

DFRobot_GNSS_I2C gnss(&I2CBackpack ,GNSS_DEVICE_ADDR);


void setup()
{
  Serial.begin(115200);
  I2CBackpack.begin(I2C_SDA, I2C_SCL, 400000); // JB
  while(!gnss.begin()){
    Serial.println("NO Deivces !");
    delay(1000);
  }

  gnss.enablePower();      // Enable gnss power 

/** Set GNSS to be used 
 *   eGPS              use gps
 *   eBeiDou           use beidou - China National Space Agency
 *   eGPS_BeiDou       use gps + beidou China National Space Agency
 *   eGLONASS          use glonass
 *   eGPS_GLONASS      use gps + glonass
 *   eBeiDou_GLONASS   use beidou +glonass China National Space Agency
 *   eGPS_BeiDou_GLONASS use gps + beidou + glonass China National Space Agency
 */
  gnss.setGnss(eGPS_GLONASS); // What do these each do?


  // gnss.setRgbOff();
  gnss.setRgbOn();
  // gnss.disablePower();      // Disable GNSS, the data will not be refreshed after disabling  
}

void loop()
{
  sTim_t utc = gnss.getUTC();
  sTim_t date = gnss.getDate();
  sLonLat_t lat = gnss.getLat();
  sLonLat_t lon = gnss.getLon();
  double high = gnss.getAlt();
  uint8_t starUserd = gnss.getNumSatUsed();
  double sog = gnss.getSog();
  double cog = gnss.getCog();

  Serial.println("");
  Serial.print(date.year);
  Serial.print("/");
  Serial.print(date.month);
  Serial.print("/");
  Serial.print(date.date);
  Serial.print("/");
  Serial.print(utc.hour);
  Serial.print(":");
  Serial.print(utc.minute);
  Serial.print(":");
  Serial.print(utc.second);
  Serial.println();
  Serial.println((char)lat.latDirection);
  Serial.println((char)lon.lonDirection);
  
  // Serial.print("lat DDMM.MMMMM = ");
  // Serial.println(lat.latitude, 5);
  // Serial.print(" lon DDDMM.MMMMM = ");
  // Serial.println(lon.lonitude, 5);
  Serial.print("lat degree = ");
  Serial.println(lat.latitudeDegree,6);
  Serial.print("lon degree = ");
  Serial.println(lon.lonitudeDegree,6);

  Serial.print("star userd = ");
  Serial.println(starUserd);
  Serial.print("alt high = ");
  Serial.println(high);
  Serial.print("sog =  ");
  Serial.println(sog);
  Serial.print("cog = ");
  Serial.println(cog);
  Serial.print("gnss mode =  ");
  Serial.println(gnss.getGnssMode());
  delay(1000);
}