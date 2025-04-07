#include "insole_sensor.h"

#include <Arduino.h>
#include <cstring>

InsoleSensor::InsoleSensor(const double firstDividerResistance, const double secondDividerResistance, const double inputVoltage, const std::array<uint8_t, NUM_INSOLE_PRESSURE> digitalPinMapping, const std::array<uint8_t, NUM_INSOLE_PRESSURE> analogPinMapping) {
    for (uint8_t i = 0; i < NUM_INSOLE_PRESSURE; ++i) {
        double dividerResistance = i < 6 ? firstDividerResistance : secondDividerResistance;
        VoltageDivider divider(analogPinMapping[i], dividerResistance, inputVoltage);
        m_voltageDividers[i] = divider;
    }

    std::memcpy(&m_analogPinMapping, &analogPinMapping, sizeof(m_analogPinMapping));
    std::memcpy(&m_digitalPinMapping, &digitalPinMapping, sizeof(m_digitalPinMapping));

}

bool InsoleSensor::init() {
    bool ok = true;
    for (uint8_t i = 0; i < NUM_INSOLE_PRESSURE; ++i) {
        ok |= m_voltageDividers[i].init();
        // Input sets the pin to high impedance so it will not sink or source any current
        pinMode(m_digitalPinMapping[i], INPUT);
    }
    return ok;
}

void InsoleSensor::readData() {
    for (uint8_t i = 0; i < NUM_INSOLE_PRESSURE; ++i) {
        pinMode(m_digitalPinMapping[i], OUTPUT);
        digitalWrite(m_digitalPinMapping[i], HIGH);
        delay(10);
        m_voltageDividers[i].readData();
        std::memcpy(&m_sensorData.insolePressures[i], &m_voltageDividers[i].getData(), sizeof(m_sensorData.insolePressures[i]));
        pinMode(m_digitalPinMapping[i], INPUT);
    }
}