#pragma once
#include <Adafruit_MPU6050.h>
#include "types.h"

class ImuSensor {
public:
  bool begin();
  void calibrate(int samples = 200);
  void resetZero();
  bool update();
  SkiTelemetryPacket getTelemetry() const;

private:
  Adafruit_MPU6050 mpu_;
  unsigned long lastMicros_ = 0;

  float angleX_ = 0.0f;
  float angleY_ = 0.0f;
  float angleZ_ = 0.0f;

  float offsetX_ = 0.0f;
  float offsetY_ = 0.0f;
  float offsetGz_ = 0.0f;
};