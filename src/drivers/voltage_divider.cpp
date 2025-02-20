#include "voltage_divider.h"

#include <Arduino.h>

// Has to be board-wide configuration, so not a parameter to Voltage Divider
constexpr const double REFERENCE_VOLTAGE = 3.0;

VoltageDivider::VoltageDivider(const uint32_t analogPin, const double dividerResistance, const double inputVoltage, const uint32_t adcMax): 
    m_analogPin{analogPin}, m_dividerResistance{dividerResistance}, m_inputVoltage{inputVoltage}, m_adcMax{adcMax} {
        
    }

bool VoltageDivider::init() {
    pinMode(m_analogPin, INPUT);
    // Must match REFERENCE_VOLTAGE
    analogReference(AR_INTERNAL_3_0);
    analogReadResolution(12);
    return true;
}

void VoltageDivider::readData() {
    uint32_t rawValue = analogRead(m_analogPin);
    uint32_t scaledMax = (m_inputVoltage / REFERENCE_VOLTAGE) * m_adcMax;
    double readVoltage = ((double)rawValue / (double)scaledMax) * m_inputVoltage;

    double readResistance = ((m_inputVoltage / readVoltage) - 1) * m_dividerResistance;

    m_sensorData.adcRawCount = rawValue;
    m_sensorData.calculatedResistance = readResistance;
    m_sensorData.dividerResistance = m_dividerResistance;
    m_sensorData.inputVoltage = m_inputVoltage;
    m_sensorData.outputVoltage = readVoltage;
}