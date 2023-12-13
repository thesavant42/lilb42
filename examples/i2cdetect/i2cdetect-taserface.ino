/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/
#include <Arduino.h>
#include <Wire.h>

TwoWire I2CBackpack = TwoWire(0);

void setup() {
  I2CBackpack.begin(I2C_SDA, I2C_SCL, 400000);
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
  Serial.println("\nI2C_SDA:");
  Serial.println(I2C_SDA);
  Serial.println("\nI2C_SCL");
  Serial.println(I2C_SCL);
}
 
void loop() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    I2CBackpack.beginTransmission(address);
    error = I2CBackpack.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}