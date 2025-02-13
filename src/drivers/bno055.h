#ifndef SRC_DRIVERS_BNO055_H_
#define SRC_DRIVERS_BNO055_H_

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#include "datatypes.h"
#include "sensor.h"

class BNO055: public Sensor<ImuData> {
  public:
    /**
     * @brief Construct BNO055
     * @param sensorID Sensor ID
     * @param address I2C address of sensor
     * @param wire I2C interface sensor is connected to
     * @param debugPrint Enable or disable debug printing of data
     */
    BNO055(const int32_t sensorID, const uint8_t address, TwoWire *wire, const bool debugPrint);

    virtual bool init() override;

    virtual void readData() override;

  private:

    void handleSensorEvent(const sensors_event_t& event);

    Adafruit_BNO055 m_bno;

    const bool m_debugPrint;
};

#endif // SRC_DRIVERS_BNO055_H_