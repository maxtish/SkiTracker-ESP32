#include "imu_sensor.h"
#include "config.h"
#include <Wire.h>
#include <math.h>

static constexpr float RAD_TO_DEG_F = 57.2957795f;

bool ImuSensor::begin() {
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(I2C_CLOCK_HZ);
  delay(100);

  if (!mpu_.begin(0x68, &Wire) && !mpu_.begin(0x69, &Wire)) {
    return false;
  }

  mpu_.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu_.setGyroRange(MPU6050_RANGE_1000_DEG);
  mpu_.setFilterBandwidth(MPU6050_BAND_44_HZ);

  calibrate();
  lastMicros_ = micros();
  return true;
}

void ImuSensor::calibrate(int samples) {
  float sumX = 0.0f, sumY = 0.0f, sumGz = 0.0f;

  for (int i = 0; i < samples; i++) {
    sensors_event_t a, g, t;
    mpu_.getEvent(&a, &g, &t);
    sumX += atan2f(a.acceleration.y, a.acceleration.z) * RAD_TO_DEG_F;
    sumY += atan2f(-a.acceleration.x, a.acceleration.z) * RAD_TO_DEG_F;
    sumGz += g.gyro.z;
    delay(5);
  }

  offsetX_ = sumX / samples;
  offsetY_ = sumY / samples;
  offsetGz_ = sumGz / samples;
  angleX_ = angleY_ = angleZ_ = 0.0f;
}

void ImuSensor::resetZero() {
  offsetX_ += angleX_;
  offsetY_ += angleY_;
  angleX_ = angleY_ = angleZ_ = 0.0f;
}

bool ImuSensor::update() {
  unsigned long now = micros();
  if (now - lastMicros_ < IMU_INTERVAL_US) return false;
  float dt = (now - lastMicros_) / 1000000.0f;
  lastMicros_ = now;

  sensors_event_t a, g, t;
  mpu_.getEvent(&a, &g, &t);

  float accX = atan2f(a.acceleration.y, a.acceleration.z) * RAD_TO_DEG_F - offsetX_;
  float accY = atan2f(-a.acceleration.x, a.acceleration.z) * RAD_TO_DEG_F - offsetY_;

  angleX_ = COMP_FILTER_ALPHA * (angleX_ + (g.gyro.x * RAD_TO_DEG_F) * dt) + (1.0f - COMP_FILTER_ALPHA) * accX;
  angleY_ = COMP_FILTER_ALPHA * (angleY_ + (g.gyro.y * RAD_TO_DEG_F) * dt) + (1.0f - COMP_FILTER_ALPHA) * accY;
  angleZ_ += (g.gyro.z - offsetGz_) * RAD_TO_DEG_F * dt;

  return true;
}

SkiTelemetryPacket ImuSensor::getTelemetry() const {
  return { millis(), angleX_, angleY_, angleZ_ };
}