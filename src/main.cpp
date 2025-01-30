#include <Arduino.h>
#include <bluefruit.h>

#define MANUFACTURER_ID   0x0822

// UUID for peripheral device
uint8_t beaconUuid[16] = {
  0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
  0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0
};

// UUID, Major, Minor, RSSI @ 1M
BLEBeacon beacon(beaconUuid, 1, 2, -54);

void startAdvertising();

void setup() {
  Serial.begin(115200);

  Serial.println("Sensor/beacon test");

  Bluefruit.begin();
  Bluefruit.setTxPower(0); // Check bluefruit.h for supported values

  // Manufacturer ID is required for Manufacturer Specific Data
  beacon.setManufacturer(MANUFACTURER_ID);

  startAdvertising();

  Serial.printf("Broadcasting beacon with MANUFACTURER_ID = 0x%04X\n", MANUFACTURER_ID);

  // Loop doesn't do anything for now
  suspendLoop();
}

void startAdvertising()
{
  // Set beacon payload
  Bluefruit.Advertising.setBeacon(beacon);

  // Add name packet after advertising packet
  Bluefruit.ScanResponse.addName();

  // Start advertising
  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(160, 160); // Unit 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30); // Fast mode window (secs)
  Bluefruit.Advertising.start(0); // Seconds to advertise for, 0 = unlimited
}

void loop() {
  // Loop is suspended
}