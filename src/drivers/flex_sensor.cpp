#include "flex_sensor.h"

#include <Arduino.h>
#include <cstring>

FlexSensor::FlexSensor(const uint32_t analogPin, const double dividerResistance, const double inputVoltage):
    m_voltageDivider{analogPin, dividerResistance, inputVoltage} {

    }

bool FlexSensor::init() {
    return m_voltageDivider.init();
}

void FlexSensor::readData() {
    m_voltageDivider.readData();
    std::memcpy(&m_sensorData, &m_voltageDivider.getData(), sizeof(m_sensorData));
}