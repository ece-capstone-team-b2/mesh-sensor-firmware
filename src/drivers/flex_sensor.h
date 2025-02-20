#ifndef SRC_DRIVERS_FLEX_SENSOR_H_
#define SRC_DRIVERS_FLEX_SENSOR_H_

#include "sensor.h"
#include "voltage_divider.h"

class FlexSensor: public Sensor<FlexData> {
  public:
    /**
     * @brief Construct a flex sensor object
     * @param analogPin The analog pin connected to the voltage divider
     * @param dividerResistance The resistor resistance in ohms used as the divider with the flex sensor
     * @param inputVoltage Input voltage to the divider
     */
    FlexSensor(const uint32_t analogPin, const double dividerResistance, const double inputVoltage);

    virtual bool init() override;

    virtual void readData() override;
  private:
    VoltageDivider m_voltageDivider;
};


#endif // SRC_DRIVERS_FLEX_SENSOR_H_