#include <Arduino.h>
#include <Wire.h>
#include <bluefruit.h>

#include "bno055.h"
#include "insole_sensor.h"
#include "flex_sensor.h"
#include "ble_node.h"


BNO055 bnoImu{55, 0x28, &Wire, false};
FlexSensor flexSensor{A0, 50000, 3.3};


void setup(void)
{
  Serial.begin(9600);
  if (bnoImu.init()) {
    Serial.printf("IMU Initialized\n");
  }
  if (flexSensor.init()) {
    Serial.printf("Flex Sensor Initialized\n");
  }

  bluetooth_node::init();
}

void loop() {
  bnoImu.readData();
  flexSensor.readData();
  auto imuData = bnoImu.getData();
  
  BlePacket<ImuData> imuPacket {sizeof(BlePacket<ImuData>), PacketType::ImuPacket, BLE_ID, imuData, 0};

  bluetooth_node::transmit(imuPacket);

  Serial.printf("Imu Accel: %f x, %f y, %f z\n", imuData.accelData.x, imuData.accelData.y, imuData.accelData.z);
  auto flexData = flexSensor.getData();
  BlePacket<FlexData> flexPacket {sizeof(BlePacket<FlexData>), PacketType::KneeFlex, BLE_ID, flexData, 0};
  bluetooth_node::transmit(flexPacket);
  Serial.printf("Flex Sensor: Resistance %f \n", flexData.flexData.calculatedResistance);
  
  delay(1);
}