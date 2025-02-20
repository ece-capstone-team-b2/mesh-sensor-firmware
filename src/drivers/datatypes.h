#ifndef SRC_DRIVERS_DATATYPES_H_
#define SRC_DRIVERS_DATATYPES_H_

#include <inttypes.h>

#define packed_struct struct __attribute__((packed))

/**
 * Templated base struct for 3d axis data
 */
template<typename T>
packed_struct Axis3d {
    T x;
    T y;
    T z;
};

/**
 * Quaternion orientation representation
 */
packed_struct Quaternion {
    double w;
    double x;
    double y;
    double z;
};


/**
 * Euler angles orientation representation
 */
packed_struct EulerAngles {
    double roll;
    double pitch;
    double yaw;
};

/**
 * Position information about a sensor in the network
 */
packed_struct PositionData {
    Axis3d<double> position;
    Quaternion quatOrientation;
    EulerAngles eulerOrientation;
};

/**
 * All data reported by IMU
 */
packed_struct ImuData {
    Axis3d<double> accelData; // m/s^2
    Axis3d<double> linearAccelData; // m/s^2, removed gravity
    Axis3d<double> gravityAccel; // m/s^2, just gravity vector
    Axis3d<double> gyroData; // rad/s
    Axis3d<double> magData; // uT
    PositionData positionData;
    // Calibration values range from 0 to 3, 3 being calibrated
    uint8_t sysCalibration; 
    uint8_t accelCalibration;
    uint8_t gyroCalibration;
    uint8_t magCalibration;
};

/** 
 * Information reported by a voltage divider
 */
packed_struct VoltageDividerData {
    uint32_t adcRawCount; // Raw ADC data
    double inputVoltage; // Input voltage to volage divider
    double dividerResistance; // Resistance of the known resistor value
    double outputVoltage; // Output voltage, scaled raw counts
    double calculatedResistance; // Calculated resistance of divided component
};


/**
 * Flex sensor data
 */
packed_struct FlexData {
    VoltageDividerData flexData;
};

const constexpr uint8_t NUM_INSOLE_PRESSURE = 8;

/**
 * Insole sensor data
 * 
 * Has 8 internal voltage dividers
 */
packed_struct InsoleData {
    VoltageDividerData insolePressures[NUM_INSOLE_PRESSURE];
};

#endif // SRC_DRIVERS_DATATYPES_H_