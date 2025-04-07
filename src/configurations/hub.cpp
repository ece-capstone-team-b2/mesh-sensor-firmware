#include <Arduino.h>
#include <bluefruit.h>


#include <vector>
#include <variant>

#include "datatypes.h"
#include "ble_hub.h"

void setup()
{
    Serial.begin(115200);

    
    bluetooth_hub::init();
    
}

void loop()
{
    bluetooth_hub::loop();

    delay(1);
}
