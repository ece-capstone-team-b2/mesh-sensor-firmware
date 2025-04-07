#include <Arduino.h>
#include <Wire.h>
#include <bluefruit.h>

#include "bno055.h"
#include "insole_sensor.h"
#include "flex_sensor.h"
#include "ble_node.h"


BNO055 bnoImu{55, 0x28, &Wire, false};
// FlexSensor flexSensor{A0, 50000, 3.0};
// TODO Update these pins to the correct ones
const std::array<uint8_t, NUM_INSOLE_PRESSURE> insoleDigitalPins{6, 9,10,11,12, A3, A4,A5};
const std::array<uint8_t, NUM_INSOLE_PRESSURE> insoleAnalogPins{A1,A1,A1,A1,A1,A1,A2,A2};
InsoleSensor insoleSensor{50000,5000, 3.3, insoleDigitalPins, insoleAnalogPins};


void setup(void)
{
  Serial.begin(9600);
  if (bnoImu.init()) {
    Serial.printf("IMU Initialized\n");
  }

  if (insoleSensor.init()) {
    Serial.printf("Insole Sensor Initialized\n");
  }
  bluetooth_node::init();
}

void loop() {
  bnoImu.readData();
  insoleSensor.readData();
  auto insoleData = insoleSensor.getData();
  auto imuData = bnoImu.getData();

  Serial.printf("Imu Orientation: %.10f,%.10f,%.10f,%.10f\n",imuData.positionData.quatOrientation.w,  imuData.positionData.quatOrientation.x,  imuData.positionData.quatOrientation.y,  imuData.positionData.quatOrientation.z);

  BlePacket<ImuData> imuPacket {sizeof(BlePacket<ImuData>), PacketType::ImuPacket, BLE_ID, imuData, 0};

  bluetooth_node::transmit(imuPacket);

  BlePacket<InsoleData> insolePacket {sizeof(BlePacket<InsoleData>), PacketType::InsoleForce, BLE_ID, insoleData, 0};

  bluetooth_node::transmit(insolePacket);
  Serial.printf("Insole Data: \n");
  for (int i = 0; i < NUM_INSOLE_PRESSURE; ++i) {
      Serial.printf("%d: %d, %f\n", i, insoleData.insolePressures[i].adcRawCount, insoleData.insolePressures[i].calculatedResistance);
  }

  delay(1);
}