#ifndef SRC_DRIVERS_VOLTAGE_DIVIDER_H_
#define SRC_DRIVERS_VOLTAGE_DIVIDER_H_

#include "datatypes.h"
#include "sensor.h"

class VoltageDivider: public Sensor<VoltageDividerData>  {
    public:
    /**
     * @brief Construct a voltage divider object
     * 
     * @details This describes a voltage divider where an input voltage is connected across the device to be measured
     *          which then connects across a known resistance value to ground.
     * 
     * @param analogPin The analog pin connected to the voltage divider
     * @param dividerResistance The resistor resistance in ohms used as the divider with the flex sensor
     * @param inputVoltage Voltage input to the divider
     * @param adcMax Maximum ADC input value (default 4095)
     */
    VoltageDivider(const uint32_t analogPin, const double dividerResistance, const double inputVoltage, const uint32_t adcMax = 4095);

    virtual bool init() override;

    virtual void readData() override;

  private:
    const uint32_t m_analogPin;
    const double m_dividerResistance;
    const double m_inputVoltage;
    const uint32_t m_adcMax;
};


#endif // SRC_DRIVERS_VOLTAGE_DIVIDER_H_