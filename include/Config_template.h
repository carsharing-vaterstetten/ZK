#pragma once

#include "Log.h"

// Serial debugging
#define ENABLE_SERIAL_LOGGING true // Enable for serial printing
#define COLORIZE_SERIAL_LOGGING true
#if ENABLE_SERIAL_LOGGING
#define SERIAL_LOGGING_LEVEL LOGGING_LEVEL_DEBUG
#endif
#define FLASH_LOGGING_LEVEL LOGGING_LEVEL_INFO

#define UART_BAUD 115200U

#define RECORD_GPS_WHILE_STANDING false
#define GPS_UPDATE_INTERVAL_WHILE_DRIVING 500 // ms
#define GPS_UPDATE_INTERVAL_WHILE_STANDING 1000 // ms

#define LED_PIN 12
#define LED_COUNT 4

// Modem pins
#define MODEM_DTR_PIN           (32)
#define MODEM_RX_PIN            (26)
#define MODEM_TX_PIN            (27)
#define BOARD_POWERON_PIN       (25)
#define BOARD_PWRKEY_PIN        (4)

#define MODEM_POWERON_PULSE_WIDTH_MS      (1000)
#define MODEM_POWEROFF_PULSE_WIDTH_MS     (1300)

// Car key pins
#define OPEN_KEY 15
#define CLOSE_KEY 14

// NFC modul Pins
#define NFC_MOSI 23
#define NFC_MISO 19
#define NFC_SCLK 18
#define NFC_SS 5
#define NFC_SPI HSPI

// Restart the esp at this time
#define TARGET_TIME_FOR_ESP_RESTART 12600000U // [ms] = (3 * 3600 + 30 * 60) * 1000 -> 03:30 AM

// Check for firmware update on boot. Useful for development
#define CHECK_FOR_FIRMWARE_UPDATE_ON_BOOT true

// Hardware watchdog
#define HW_WATCHDOG_INITIAL_STARTUP_TIMEOUT 1200U // [s]
#define HW_WATCHDOG_DEFAULT_TIMEOUT 300U // [s] If the watchdog doesn't get reset in this time, it will restart the esp
#define HW_WATCHDOG_RESET_DELAY_MS 100U // [ms] reset the watchdog after this time

// Connection speed test
#define GIVE_CONNECTION_SPEED_ESTIMATE false
#define CONNECTION_SPEED_TEST_FILE_SIZE 16384 // Smaller files result in less accurate connection speed estimates. Number must not be (16 * 1024)!!! to convert it to a string literal!!!

// Config override
#define USE_DEFAULT_CONFIG false // Useful for development or mass deployment
// Default config values are used when no config was previously set and before the user entered a config. Or when USE_DEFAULT_CONFIG is true
#define DEFAULT_CONFIG_APN ""
#define DEFAULT_CONFIG_GPRS_USER ""
#define DEFAULT_CONFIG_GPRS_PASSWORD ""
#define DEFAULT_CONFIG_SERVER ""
#define DEFAULT_CONFIG_PORT 80
#define DEFAULT_CONFIG_PASSWORD ""
#define DEFAULT_SIM_PIN ""
