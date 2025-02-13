#ifndef SRC_SENSOR_H_
#define SRC_SENSOR_H_

#include <inttypes.h>
#include <tuple>

/**
 * @brief Sensor base class
 * @tparam TSensorData The type of data exposed by the sensor
 */
template <typename TSensorData>
class Sensor {
  public:
    /**
     * @brief Initialize the sensor object
     * @return True if initialization succeeds
     */
    virtual bool init() = 0;

    /**
     * @brief Read data from the sensor
     */
    virtual void readData() = 0;

    /**
     * @brief Get the internal sensor data struct
     * @return Sensor data struct
     */
    const TSensorData& getData() const  {
        return m_sensorData;
    }

  protected:
    TSensorData m_sensorData;
};

#endif // SRC_SENSOR_H_