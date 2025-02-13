#include <Arduino.h>
#include <bluefruit.h>

BLEClientBas clientBas; // battery client
BLEClientDis clientDis; // Device information client
BLEClientUart clientUart; // BLE UART client

void scan_callback(ble_gap_evt_adv_report_t* report);
void connect_callback(uint16_t conn_handle);
void disconnect_callback(uint16_t conn_handle, uint8_t reason);
void bleuart_rx_callback(BLEClientUart& uart_svc);

void setup()
{
    Serial.begin(115200);

    Serial.println("Starting Bluefruit Hub");

    // Initialize Bluefruit with 1 peripheral and 1 central node
    Bluefruit.begin(0, 1);
    Bluefruit.setName("Hub");

    Serial.println("Starting clients");
    // Initialize clients
    clientBas.begin();
    clientDis.begin();
    clientUart.begin();
    clientUart.setRxCallback(bleuart_rx_callback);

    // Increase Blink rate to different from PrPh advertising mode
    Bluefruit.setConnLedInterval(250);

    // Set callbacks on device connect and disconnect
    Serial.println("Setting callbacks");
    Bluefruit.Central.setConnectCallback(connect_callback);
    Bluefruit.Central.setDisconnectCallback(disconnect_callback);
    Bluefruit.Scanner.setRxCallback(scan_callback);

    // Start scanning
    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.setInterval(160, 80); // Scan every 160 units for 80 units where unit == 0.625 ms
    Bluefruit.Scanner.useActiveScan(false);
    Bluefruit.Scanner.start(0); // Timeout to stop scanning after, 0 is forever
}

// Callback when scan detects device
void scan_callback(ble_gap_evt_adv_report_t* report)
{
    // Check if advertising contain BLE UART service
    if (Bluefruit.Scanner.checkReportForService(report, clientUart))
    {
        Serial.print("BLE UART service detected, connecting");

        // Connect to device
        Bluefruit.Central.connect(report);
    }
    else
    {
        // Scanner pauses after receiving a report; resume if connection isn't made
        Serial.print("Connection wasn't made, resuming scan");
        Bluefruit.Scanner.resume();
    }
}

// Callback after connection is made
void connect_callback(uint16_t conn_handle)
{
    Serial.println("Connect callback called");

    if (clientDis.discover(conn_handle))
    {
        Serial.println("Found device handle");
        char buffer[32 + 1];

        // Get device manufacturer
        memset(buffer, 0, sizeof(buffer));
        if (clientDis.getManufacturer(buffer, sizeof(buffer)))
        {
            Serial.print("Manufacturer: ");
            Serial.println(buffer);
        }

        // Get device model
        memset(buffer, 0, sizeof(buffer));
        if (clientDis.getModel(buffer, sizeof(buffer)))
        {
            Serial.print("Model: ");
            Serial.println(buffer);
        }

        Serial.println();
    }
    else
    {
        Serial.println("No device handle found");
    }

    if (clientBas.discover(conn_handle))
    {
        Serial.print("Got device battery level: ");
        Serial.print(clientBas.read());
        Serial.println("%");
    }

    if (clientUart.discover(conn_handle))
    {
        Serial.println("Got device UART service");

        Serial.println("Enable TXD's notify");
        clientUart.enableTXD();

        Serial.println("Ready to receive from peripheral");
    }
    else
    {
        Serial.println("No UART service found, disconnecting");
        Bluefruit.disconnect(conn_handle);
    }
}

// Callback after disconnect
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    (void) conn_handle;
    (void) reason;

    Serial.print("Disconnected, reason = 0x");
    Serial.println(reason, HEX);
}

// Callback on UART receive
void bleuart_rx_callback(BLEClientUart& uart_svc)
{
    Serial.print("[RX]: ");

    while (uart_svc.available())
    {
        Serial.print((char) uart_svc.read());
    }

    Serial.println();
}

void loop()
{
    if (Bluefruit.Central.connected())
    {
        if (clientUart.discovered())
        {
            // Discovered means in working state
            // Get Serial input and send to Peripheral
            if (Serial.available())
            {
                Serial.println("Sending serial input to peripheral");
                delay(2); // delay a bit for all characters to arrive

                char str[20 + 1] = {0};
                Serial.readBytes(str, 20);

                clientUart.print(str);
            }
        }
    }
    else
    {
        Serial.println("Searching for device...");
    }

    delay(1);
}
