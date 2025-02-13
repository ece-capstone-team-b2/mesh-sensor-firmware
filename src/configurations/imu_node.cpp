#include <Arduino.h>
#include <Wire.h>
#include <bluefruit.h>

#include "bno055.h"

BNO055 bno_imu{55, 0x28, &Wire, false};

void setup(void)
{
  Serial.begin(9600);
  while(!Serial) {}
  // Serial.println("hello");
  if (!bno_imu.init()) {
    Serial.println("BNO055 failed to initialize");
  }
}

void loop() {
  bno_imu.readData();
  Serial.write((uint8_t*)(&bno_imu.getData()), sizeof(bno_imu.getData()));
 // Serial.println("hellowe");
  delay(100);
}