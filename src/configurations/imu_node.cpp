#include <Arduino.h>
#include <Wire.h>
#include <bluefruit.h>

#include "bno055.h"
#include "insole_sensor.h"
#include "flex_sensor.h"


BNO055 bnoImu{55, 0x28, &Wire, false};
FlexSensor flexSensor{A0, 50000, 3.0};
// TODO Update these pins to the correct ones
const std::array<uint8_t, NUM_INSOLE_PRESSURE> insoleDigitalPins{6, 9,10,11,12, A3, A4,A5};
const std::array<uint8_t, NUM_INSOLE_PRESSURE> insoleAnalogPins{A1,A1,A1,A1,A1,A1,A2,A2};
InsoleSensor insoleSensor{47000,4700, 3.0, insoleDigitalPins, insoleAnalogPins};


void setup(void)
{
  Serial.begin(9600);
  while(!Serial) {}
  if (bnoImu.init()) {
    Serial.printf("IMU Initialized\n");
  }
  if (flexSensor.init()) {
    Serial.printf("Flex Sensor Initialized\n");
  }
  if (insoleSensor.init()) {
    Serial.printf("Insole Sensor Initialized\n");
  }
}

void loop() {
  bnoImu.readData();
  flexSensor.readData();
  insoleSensor.readData();
  auto imuData = bnoImu.getData();
  Serial.printf("Imu Accel: %f x, %f y, %f z\n", imuData.accelData.x, imuData.accelData.y, imuData.accelData.z);
  auto flexData = flexSensor.getData();
  Serial.printf("Flex Sensor: Resistance %f \n", flexData.flexData.calculatedResistance);
  auto data = insoleSensor.getData();
  for (uint8_t i = 0; i < 8; ++i) {
    Serial.printf("Insole sensor Pad %d: ", i);
    if (data.insolePressures[i].calculatedResistance > 1000000) {
      Serial.printf("Resistance infinite\n");
    } else {
      Serial.printf("Resistance %f\n", data.insolePressures[i].calculatedResistance);
    }

  }
  
  delay(100);
}