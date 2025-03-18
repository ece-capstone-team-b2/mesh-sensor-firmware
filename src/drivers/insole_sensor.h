#ifndef SRC_DRIVERS_INSOLE_SENSOR_H_
#define SRC_DRIVERS_INSOLE_SENSOR_H_

#include "sensor.h"
#include "voltage_divider.h"

class InsoleSensor: public Sensor<InsoleData> {
  public:
    /**
     * @brief Construct an insole sensor object
     * @param firstDividerResistance The resistor resistance in ohms used as the divider for the first 6 non-heel sensors
     * @param secondDividerResistance The resistor resistance in ohms used as the divider for the 2 heel sensors
     * @param inputVoltage Input voltage to the divider
     * @param digitalPinMapping Mappings between pressure sensor number and digital pin to write to read from it
     * @param analogPinMapping Mapping between pressure sensor number and analog pin to read to receive data
     */
    InsoleSensor(const double firstDividerResistance, const double secondDividerResistance, const double inputVoltage, const std::array<uint8_t, NUM_INSOLE_PRESSURE> digitalPinMapping, const std::array<uint8_t, NUM_INSOLE_PRESSURE> analogPinMapping);

    virtual bool init() override;

    virtual void readData() override;

  private:
    // Mappings between pressure sensor number and digital pin to write to read from it
    std::array<uint8_t, NUM_INSOLE_PRESSURE> m_digitalPinMapping;
    // Mapping between pressure sensor number and analog pin to read to receive data
    std::array<uint8_t, NUM_INSOLE_PRESSURE> m_analogPinMapping;

    VoltageDivider m_voltageDividers[NUM_INSOLE_PRESSURE];
};


#endif // SRC_DRIVERS_INSOLE_SENSOR_H_