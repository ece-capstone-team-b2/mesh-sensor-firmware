#include "bno055.h"

BNO055::BNO055(const int32_t sensorID, const uint8_t address, TwoWire *wire, const bool debugPrint): 
    m_bno{sensorID, address, wire}, m_debugPrint{debugPrint} {

    }

bool BNO055::init() {
    if(!m_bno.begin()) {
        return false;
    }
    // This should in theory increase sensor accuracy
    m_bno.setExtCrystalUse(true);
    return true;
}

void BNO055::readData() {

    sensors_event_t sensorEvent;
    for (const auto eventType : {Adafruit_BNO055::VECTOR_GYROSCOPE, Adafruit_BNO055::VECTOR_ACCELEROMETER, Adafruit_BNO055::VECTOR_EULER, Adafruit_BNO055::VECTOR_LINEARACCEL, Adafruit_BNO055::VECTOR_GRAVITY}) {
        m_bno.getEvent(&sensorEvent, eventType);
        handleSensorEvent(sensorEvent);
    }

    // TODO: Probably don't need to do this every loop
    uint8_t sys, gyro, accel, mag = 0;
    m_bno.getCalibration(&sys, &gyro, &accel, &mag);
    m_sensorData.sysCalibration = sys;
    m_sensorData.accelCalibration = accel;
    m_sensorData.gyroCalibration = gyro;
    m_sensorData.magCalibration = mag;
}

void BNO055::handleSensorEvent(const sensors_event_t& event) {
    if (event.type == SENSOR_TYPE_ACCELEROMETER) {
        m_sensorData.accelData.x = event.acceleration.x;
        m_sensorData.accelData.y = event.acceleration.y;
        m_sensorData.accelData.z = event.acceleration.z;
        if (m_debugPrint) {
            Serial.printf("Accel data: %f x, %f y, %f z\n", event.acceleration.x, event.acceleration.y, event.acceleration.z);
        }
   } else if (event.type == SENSOR_TYPE_LINEAR_ACCELERATION) {
        m_sensorData.linearAccelData.x = event.acceleration.x;
        m_sensorData.linearAccelData.y = event.acceleration.y;
        m_sensorData.linearAccelData.z = event.acceleration.z;
        if (m_debugPrint) {
            Serial.printf("Linear accel data: %f x, %f y, %f z\n", event.acceleration.x, event.acceleration.y, event.acceleration.z);
        }
    } else if (event.type == SENSOR_TYPE_GRAVITY) {
        m_sensorData.gravityAccel.x = event.acceleration.x;
        m_sensorData.gravityAccel.y = event.acceleration.y;
        m_sensorData.gravityAccel.z = event.acceleration.z;
        if (m_debugPrint) {
            Serial.printf("Gravity data: %f x, %f y, %f z\n", event.acceleration.x, event.acceleration.y, event.acceleration.z);
        }
    }
   else if (event.type == SENSOR_TYPE_ORIENTATION) {
        m_sensorData.positionData.eulerOrientation.roll = event.orientation.x;
        m_sensorData.positionData.eulerOrientation.pitch = event.orientation.y;
        m_sensorData.positionData.eulerOrientation.yaw = event.orientation.z;
        if (m_debugPrint) {
            Serial.printf("Euler orientation: %f roll, %f pitch, %f yaw\n", event.orientation.x, event.orientation.y, event.orientation.z);
        }
        imu::Quaternion quat = m_bno.getQuat();
        m_sensorData.positionData.quatOrientation.w = quat.w();
        m_sensorData.positionData.quatOrientation.x = quat.x();
        m_sensorData.positionData.quatOrientation.y = quat.y();
        m_sensorData.positionData.quatOrientation.z = quat.z();
        if (m_debugPrint) {
            Serial.printf("Quaternion: %f w, %f x, %f y, %f z\n", quat.w(), quat.x(), quat.y(), quat.z());
        }
   }
   else if (event.type == SENSOR_TYPE_MAGNETIC_FIELD) {
        m_sensorData.magData.x = event.magnetic.x;
        m_sensorData.magData.y = event.magnetic.y;
        m_sensorData.magData.z = event.magnetic.z;
        if (m_debugPrint) {
            Serial.printf("Magnetometer: %f x, %f y, %f z\n", event.magnetic.x, event.magnetic.y, event.magnetic.z);
        }
   }
   else if ((event.type == SENSOR_TYPE_GYROSCOPE) || (event.type == SENSOR_TYPE_ROTATION_VECTOR)) {
        m_sensorData.gyroData.x = event.gyro.x;
        m_sensorData.gyroData.y = event.gyro.y;
        m_sensorData.gyroData.z = event.gyro.z;
        if (m_debugPrint) {
            Serial.printf("Gyroscope: %f x, %f y, %f z\n", event.magnetic.x, event.magnetic.y, event.magnetic.z);
        }
   }
}