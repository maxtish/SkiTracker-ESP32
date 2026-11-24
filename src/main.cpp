#include <Arduino.h>
#include "config.h"
#include "imu_sensor.h"
#include "ble_service.h"

ImuSensor imu;
BleService ble;
unsigned long lastBleSendMs = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.print("Запуск узла: ");
  Serial.println(BLE_DEVICE_NAME);

  if (!imu.begin()) {
    Serial.println("[ОШИБКА] MPU-6050 не найден! Проверь пины SDA/SCL и питание 3.3V.");
    while (true) {
      delay(1000);
    }
  }
  Serial.println("[IMU] MPU-6050 готов и откалиброван.");

  ble.begin();
}

void loop() {
  // 1. Обработка команд с телефона
  if (ble.consumeResetRequest()) {
    imu.resetZero();
    Serial.println("[CMD] Ноль сброшен");
  }
  if (ble.consumeRecalibrateRequest()) {
    imu.calibrate();
    Serial.println("[CMD] Выполнена полная калибровка");
  }

  // 2. Обновление датчика (200 Гц)
  imu.update();

  // 3. Отправка пакета по BLE (25 Гц)
  unsigned long nowMs = millis();
  if (ble.isConnected() && (nowMs - lastBleSendMs >= BLE_INTERVAL_MS)) {
    lastBleSendMs = nowMs;
    ble.sendTelemetry(imu.getTelemetry());
  }
}