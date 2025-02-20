#include <Arduino.h>
#include <Wire.h>
#include <bluefruit.h>

#include "bno055.h"
#include "insole_sensor.h"
#include "flex_sensor.h"


BNO055 bnoImu{55, 0x28, &Wire, false};
FlexSensor flexSensor{A0, 39000, 3.0};
const std::array<uint8_t, NUM_INSOLE_PRESSURE> insoleDigitalPins{5,6,9,10,11,12,A4,A5};
const std::array<uint8_t, NUM_INSOLE_PRESSURE> insoleAnalogPins{A1,A1,A1,A1,A1,A1,A2,A2};
InsoleSensor insoleSensor{39000, 3.0, insoleDigitalPins, insoleAnalogPins};


void setup(void)
{
  Serial.begin(9600);
  while(!Serial) {}
  // Serial.println("hello");
  if (!bnoImu.init()) {
    // Serial.println("BNO055 failed to initialize");
  }
  flexSensor.init();
  insoleSensor.init();
}

void loop() {
  bnoImu.readData();
  flexSensor.readData();
  insoleSensor.readData();
  auto data = insoleSensor.getData();
  for (uint8_t i = 0; i < NUM_INSOLE_PRESSURE; ++i) {
    Serial.printf("Insole sensor %d:\n", i);
    Serial.printf("Adc raw: %d\n", data.insolePressures[i].adcRawCount);
    Serial.printf("calculatedResistance: %f\n", data.insolePressures[i].calculatedResistance);
    Serial.printf("dividerResistance: %f\n", data.insolePressures[i].dividerResistance);
    Serial.printf("inputVoltage: %f\n", data.insolePressures[i].inputVoltage);
    Serial.printf("outputVoltage: %f\n", data.insolePressures[i].outputVoltage);

  }
  
  delay(100);
}