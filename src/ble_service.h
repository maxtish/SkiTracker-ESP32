#pragma once
#include <NimBLEDevice.h>
#include "types.h"

class BleService {
public:
  void begin();
  void sendTelemetry(const SkiTelemetryPacket& pkt);
  bool isConnected() const;

  bool consumeResetRequest();
  bool consumeRecalibrateRequest();

private:
  NimBLEServer* server_ = nullptr;
  NimBLECharacteristic* telemetryChar_ = nullptr;
  NimBLECharacteristic* commandChar_ = nullptr;

  volatile bool connected_ = false;
  volatile bool resetRequested_ = false;
  volatile bool recalibrateRequested_ = false;

  friend class ServerCallbacks;
  friend class CommandCallbacks;
};