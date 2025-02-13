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


#endif // SRC_DRIVERS_DATATYPES_H_