#pragma once
#include <Arduino.h>

// Автоматическая настройка под выбранный профиль в platformio.ini
#if defined(SKI_SIDE_LEFT)
  #define BLE_DEVICE_NAME "SkiTracker_L"
  #define I2C_SDA_PIN     8
  #define I2C_SCL_PIN     9
  #define ROLL_SIGN       1.0f   // Поменять на -1.0f, если датчик закреплен зеркально
#elif defined(SKI_SIDE_RIGHT)
  #define BLE_DEVICE_NAME "SkiTracker_R"
  #define I2C_SDA_PIN     14
  #define I2C_SCL_PIN     15
  #define ROLL_SIGN       1.0f
#else
  #error "Не указана сторона лыжи! Выберите профиль ski_left_s3 или ski_right_cam."
#endif

// Адрес MPU-6050 (пин AD0 не подключен или на GND)
#define MPU_I2C_ADDR      0x68
#define I2C_CLOCK_HZ      400000

// Частоты работы
#define IMU_INTERVAL_US   5000   // 200 Гц (опрос и фильтрация датчика)
#define BLE_INTERVAL_MS   40     // 25 Гц (отправка пакетов на телефон)
#define COMP_FILTER_ALPHA 0.98f

// Общие BLE UUID (одинаковые для обеих лыж, чтобы приложение легко их находило)
#define BLE_SERVICE_UUID       "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_CHAR_TELEMETRY_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define BLE_CHAR_COMMAND_UUID   "6e400002-b5a3-f393-e0a9-e50e24dcca9e"