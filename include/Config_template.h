#pragma once

#include "Log.h"

// Serial debugging
#define ENABLE_SERIAL_LOGGING false // Enable for serial printing
#define COLORIZE_SERIAL_LOGGING true
#if ENABLE_SERIAL_LOGGING
#define SERIAL_LOGGING_LEVEL LOGGING_LEVEL_DEBUG
#endif
#define FLASH_LOGGING_LEVEL LOGGING_LEVEL_INFO
#define SD_CARD_LOGGING_LEVEL LOGGING_LEVEL_INFO

#define UART_BAUD 115200U

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // 1KiB

#define GPS_UPDATE_INTERVAL_WHILE_DRIVING 500 // ms
#define GPS_UPDATE_INTERVAL_WHILE_STANDING 1000 // ms

#define LED_PIN 12
#define LED_COUNT 4

// Modem pins
#define PIN_TX 27
#define PIN_RX 26
#define PWR_PIN 4

// Car key pins
#define OPEN_KEY 21
#define CLOSE_KEY 22

// SD Card pins
#define SD_MISO 2
#define SD_MOSI 15
#define SD_SCLK 14
#define SD_CS 13
#define SD_SPI VSPI

// NFC Modul Pins
#define NFC_MOSI 23
#define NFC_MISO 19
#define NFC_SCLK 18
#define NFC_SS 5
#define NFC_SPI HSPI

// Restart the esp at this time
#define TARGET_TIME_FOR_ESP_RESTART 12600000U // [ms] = (3 * 3600 + 30 * 60) * 1000 -> 03:30 AM

#define HW_WATCHDOG_DEFAULT_TIMEOUT 300U // [s] If the watchdog doesn't get reset in this time, it will restart the esp
#define HW_WATCHDOG_OTA_UPDATE_TIMEOUT 3600U // [s] same as HW_WATCHDOG_DEFAULT_TIMEOUT but when performing an ota update
#define HW_WATCHDOG_RESET_DELAY_MS 100U // [ms] reset the watchdog after this time
