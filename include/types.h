#pragma once
#include <stdint.h>

struct __attribute__((packed)) SkiTelemetryPacket {
  uint32_t timestampMs;
  float angleX;   // Наклон вокруг оси X (град)
  float angleY;   // Наклон вокруг оси Y (град)
  float angleZ;   // Поворот вокруг оси Z (град)
};

enum BleCommand : uint8_t {
  CMD_RESET_ZERO  = 0x01,
  CMD_RECALIBRATE = 0x02
};