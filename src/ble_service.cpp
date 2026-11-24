#include "ble_service.h"
#include "config.h"

class ServerCallbacks : public NimBLEServerCallbacks {
public:
  explicit ServerCallbacks(BleService* parent) : parent_(parent) {}

  void onConnect(NimBLEServer* pServer) override {
    parent_->connected_ = true;
    Serial.println("[BLE] Клиент подключен");
  }

  void onDisconnect(NimBLEServer* pServer) override {
    parent_->connected_ = false;
    Serial.println("[BLE] Клиент отключен, перезапуск рекламы...");
    NimBLEDevice::startAdvertising();
  }

private:
  BleService* parent_;
};

class CommandCallbacks : public NimBLECharacteristicCallbacks {
public:
  explicit CommandCallbacks(BleService* parent) : parent_(parent) {}

  void onWrite(NimBLECharacteristic* pCharacteristic) override {
    std::string val = pCharacteristic->getValue();
    if (val.empty()) return;

    uint8_t cmd = static_cast<uint8_t>(val[0]);
    if (cmd == CMD_RESET_ZERO) {
      parent_->resetRequested_ = true;
    } else if (cmd == CMD_RECALIBRATE) {
      parent_->recalibrateRequested_ = true;
    }
  }

private:
  BleService* parent_;
};

void BleService::begin() {
  NimBLEDevice::init(BLE_DEVICE_NAME);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9); // Максимальная мощность сигнала

  server_ = NimBLEDevice::createServer();
  server_->setCallbacks(new ServerCallbacks(this));

  NimBLEService* service = server_->createService(BLE_SERVICE_UUID);

  telemetryChar_ = service->createCharacteristic(
    BLE_CHAR_TELEMETRY_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );

  commandChar_ = service->createCharacteristic(
    BLE_CHAR_COMMAND_UUID,
    NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
  );
  commandChar_->setCallbacks(new CommandCallbacks(this));

  service->start();

  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  advertising->addServiceUUID(BLE_SERVICE_UUID);
  advertising->setScanResponse(true);
  advertising->start();

  Serial.print("[BLE] Запущен как: ");
  Serial.println(BLE_DEVICE_NAME);
}

void BleService::sendTelemetry(const SkiTelemetryPacket& pkt) {
  if (!connected_ || !telemetryChar_) return;
  telemetryChar_->setValue(reinterpret_cast<const uint8_t*>(&pkt), sizeof(pkt));
  telemetryChar_->notify();
}

bool BleService::isConnected() const {
  return connected_;
}

bool BleService::consumeResetRequest() {
  if (!resetRequested_) return false;
  resetRequested_ = false;
  return true;
}

bool BleService::consumeRecalibrateRequest() {
  if (!recalibrateRequested_) return false;
  recalibrateRequested_ = false;
  return true;
}