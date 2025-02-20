#include <Arduino.h>
#include <Wire.h>
#include <bluefruit.h>

#include "bno055.h"
#include "flex_sensor.h"

BNO055 bno_imu{55, 0x28, &Wire, false};
FlexSensor flex_sensor{A4, 39000, 3.0};

void setup(void)
{
  Serial.begin(9600);
  while(!Serial) {}
  // Serial.println("hello");
  if (!bno_imu.init()) {
    // Serial.println("BNO055 failed to initialize");
  }
  flex_sensor.init();
}

void loop() {
  bno_imu.readData();
  flex_sensor.readData();
  auto data = flex_sensor.getData();
  Serial.printf("Adc raw: %d\n", data.flexData.adcRawCount);
  Serial.printf("calculatedResistance: %f\n", data.flexData.calculatedResistance);
  Serial.printf("dividerResistance: %f\n", data.flexData.dividerResistance);
  Serial.printf("inputVoltage: %f\n", data.flexData.inputVoltage);
  Serial.printf("outputVoltage: %f\n", data.flexData.outputVoltage);
  delay(100);
}