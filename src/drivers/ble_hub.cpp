#ifdef BLE_HUB

#include <Arduino.h>
#include <bluefruit.h>
#include "datatypes.h"

#include "crc.h"

#include <optional>
#include <unordered_map>
#include "ble_hub.h"

#include <string>
#include <functional>


const constexpr size_t NUM_PERIPHERALS = 4;
const constexpr size_t RX_BUFFER_LEN = 255;

void scan_callback(ble_gap_evt_adv_report_t* report);
void connect_callback(uint16_t conn_handle);
void disconnect_callback(uint16_t conn_handle, uint8_t reason);
void bleuart_rx_callback(BLEClientUart& uart_svc);

// Global receive buffer for bluetooth data.
// Will be copied into the proper handler upon parsing
static uint8_t bleRxBuffer[RX_BUFFER_LEN];

struct BluetoothConnectionHandler {
    bool currentlyReceivingPacket = false;
    uint8_t currentBufferIndex = 0;
    uint8_t currentPacketLength;
    uint8_t rxBuffer[RX_BUFFER_LEN];
    uint16_t connHandle;
    BLEClientUart bleuart;
};

// Current connection count
static size_t connectionNum = 0;

static BluetoothConnectionHandler bleConnectionHandlers[NUM_PERIPHERALS];

// Get a handler with the given connHandle, if one exists.
std::optional<std::reference_wrapper<BluetoothConnectionHandler>> getHandler(uint16_t connHandle) {
    for (size_t i = 0; i < NUM_PERIPHERALS; ++i) {
        if (bleConnectionHandlers[i].connHandle == connHandle) {
            return std::ref(bleConnectionHandlers[i]);
        }
    }
    return std::nullopt;
}

// Parse a packet as a given type
template <typename TPacketType>
std::optional<BlePacket<TPacketType>> parsePacket(const uint8_t* data, const uint8_t packetLength) {
    using PacketType = BlePacket<TPacketType>;
    // Packet too short
    if ((packetLength) != sizeof(PacketType)) {
        // Serial.printf("Mismatched packet length: expected %d, received %d\n", sizeof(PacketType), packetLength);
        return std::nullopt;
    }

    BlePacket<TPacketType>* parsedPacket = (BlePacket<TPacketType>*)data;

    // Ignore crc bytes
    uint16_t calculated_crc = crc16(data, packetLength - sizeof(uint16_t));
    if (calculated_crc != parsedPacket->crc) {
        // Serial.println("Bad CRC!");
        return std::nullopt;
    }

    BlePacket<TPacketType> packetCopy = *parsedPacket;
    return packetCopy;
}


void bluetooth_hub::init() {
    Bluefruit.begin(0, 10);
    Bluefruit.setName("SWEAT Hub");

   
    for (size_t i = 0; i < NUM_PERIPHERALS; ++i) {
        bleConnectionHandlers[i].connHandle = BLE_CONN_HANDLE_INVALID;
        bleConnectionHandlers[i].bleuart.begin();
        bleConnectionHandlers[i].bleuart.setRxCallback(bleuart_rx_callback);
    }

    // Increase Blink rate to different from PrPh advertising mode
    Bluefruit.setConnLedInterval(250);

    // Set callbacks on device connect and disconnect
    Bluefruit.Central.setConnectCallback(connect_callback);
    Bluefruit.Central.setDisconnectCallback(disconnect_callback);
    Bluefruit.Scanner.setRxCallback(scan_callback);

    // Start scanning
    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.setInterval(160, 80); // Scan every 160 units for 80 units where unit == 0.625 ms
    Bluefruit.Scanner.useActiveScan(false);
    Bluefruit.Scanner.filterUuid(BLEUART_UUID_SERVICE);
    Bluefruit.Scanner.start(0); // Timeout to stop scanning after, 0 is forever
}

void bluetooth_hub::loop() {
    if (Bluefruit.Central.connected()) {
        //Serial.printf("Connected to %d peripherals\n", connectionNum);
    } else {
        // Serial.println("Searching for device...");
    }
}


// Callback when scan detects device
void scan_callback(ble_gap_evt_adv_report_t* report)
{
    Bluefruit.Central.connect(report);
}

// Callback after connection is made
void connect_callback(uint16_t conn_handle)
{
    // Find an available ID to use
  auto foundHandler  = getHandler(BLE_CONN_HANDLE_INVALID);

  // Eeek: Exceeded the number of connections !!!
  if ( !foundHandler.has_value() ) return;

  BluetoothConnectionHandler& handler = foundHandler.value();
  
  handler.connHandle = conn_handle;


  if ( handler.bleuart.discover(conn_handle) )
  {
    handler.bleuart.enableTXD();

    Bluefruit.Scanner.start(0);
  } else
  {

    // disconnect since we couldn't find bleuart service
    Bluefruit.disconnect(conn_handle);
  }  

  connectionNum++;
}

// Callback after disconnect
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    (void) conn_handle;
    (void) reason;

    connectionNum--;

    auto foundHandler  = getHandler(conn_handle);

    // idk whats up here
    if ( !foundHandler.has_value() ) return;
  
    BluetoothConnectionHandler& handler = foundHandler.value();
    handler.connHandle = BLE_CONN_HANDLE_INVALID;
    handler.currentBufferIndex = 0;
    handler.currentlyReceivingPacket = false;
    handler.currentPacketLength = 0;
}

// Callback on UART receive
void bleuart_rx_callback(BLEClientUart& uart_svc)
{
    uint16_t conn_handle = uart_svc.connHandle();
    auto foundHandler = getHandler(conn_handle);
    if (!foundHandler.has_value()) {
        return;
    }

    BluetoothConnectionHandler& handler = foundHandler.value();

    size_t available = uart_svc.available();
    if (available)
    {
        uart_svc.readBytes(bleRxBuffer, available);
    }

    for (size_t i = 0; i < available; ++i) {
        if (handler.currentlyReceivingPacket) {
            handler.rxBuffer[handler.currentBufferIndex++] = bleRxBuffer[i];
            if (handler.currentBufferIndex >= handler.currentPacketLength) {
                PacketType packetType = PacketType(handler.rxBuffer[1]);
                if (packetType == PacketType::ImuPacket) {
                    auto packet = parsePacket<ImuData>((uint8_t*)(&handler.rxBuffer), handler.currentPacketLength);
                    if (packet.has_value()) {
                        auto payload = packet.value().payload;
                        Serial.write((uint8_t*)(&packet), sizeof(packet));
                        // Serial.printf("%d: %.10f,%.10f,%.10f,%.10f\n", packet.value().transmitterId, payload.positionData.quatOrientation.w,  payload.positionData.quatOrientation.x,  payload.positionData.quatOrientation.y,  payload.positionData.quatOrientation.z);
                    }
                } else if (packetType==PacketType::InsoleForce) {
                    auto packet = parsePacket<InsoleData>((uint8_t*)(&handler.rxBuffer), handler.currentPacketLength);
                    if (packet.has_value()) {
                        auto payload = packet.value().payload;

                        Serial.write((uint8_t*)(&packet), sizeof(packet));
                        // Serial.printf("Insole %d:", packet.value().transmitterId); 
                        // for (int i = 0; i < NUM_INSOLE_PRESSURE; ++i) {
                        //     Serial.printf("%d: %f", i, payload.insolePressures[i].calculatedResistance);
                        // }
                        // Serial.println();
                    }
                } else if (packetType == PacketType::KneeFlex) {
                    auto packet = parsePacket<FlexData>((uint8_t*)(&handler.rxBuffer), handler.currentPacketLength);
                    if (packet.has_value()) {
                        auto payload = packet.value().payload;
                        Serial.write((uint8_t*)(&packet), sizeof(packet));
                        // Serial.printf("Flex %d: Resistance: %f \n", packet.value().transmitterId, payload.flexData.calculatedResistance);
                    }
                }
                handler.currentlyReceivingPacket = false;
            }
        } else {
            uint32_t len = bleRxBuffer[i];
            if (len != sizeof(BlePacket<ImuData>) && len != sizeof(BlePacket<InsoleData>) && len != sizeof(BlePacket<FlexData>)) {
                continue;
            }
            handler.currentlyReceivingPacket = true;
            handler.currentPacketLength = len;
            handler.currentBufferIndex = 0;
            handler.rxBuffer[handler.currentBufferIndex++] = bleRxBuffer[i];
            // Serial.printf("Starting new receive with packet length %d\n", handler.currentPacketLength);
        }
    }
}
#endif // BLE_HUB
