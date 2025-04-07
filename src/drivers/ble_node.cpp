#ifdef BLE_NODE
#include <Arduino.h>
#include <bluefruit.h>
#include <iomanip>
#include <string>

#include "datatypes.h"
#include "crc.h"
#include "ble_node.h"

BLEUart bleuart; // uart over ble

void connect_callback(uint16_t conn_handle);
void disconnect_callback(uint16_t conn_handle, uint8_t reason);
void startAdvertising();

void bluetooth_node::init()
{

  Serial.println("Starting BLE UART peripheral");

  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setTxPower(0); // Check bluefruit.h for supported values
  Bluefruit.setName("Peripheral 0"); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);


  // Configure and Start BLE Uart Service
  bleuart.begin();

  // Set up and start advertising
  startAdvertising();
}

void startAdvertising()
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   *
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

template <typename TDataType>
void bluetooth_node::transmit(BlePacket<TDataType>& packet) {
    size_t len = sizeof(packet);
    uint8_t* data = (uint8_t*)(&packet);
    uint16_t crc = crc16(data, len - sizeof(uint16_t));
    packet.crc = crc;
    bleuart.write(data, len);
}

template void bluetooth_node::transmit<ImuData>(BlePacket<ImuData>& packet);
template void bluetooth_node::transmit<FlexData>(BlePacket<FlexData>& packet);
template void bluetooth_node::transmit<InsoleData>(BlePacket<InsoleData>& packet);

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}
#endif // BLE_NODE