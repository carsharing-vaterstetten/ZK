#pragma once

#define ENABLE_SERIAL_LOGGING true // Enable for serial printing
#define COLORIZE_SERIAL_LOGGING true
// Serial debugging
#if ENABLE_SERIAL_LOGGING
#define SERIAL_LOGGING_LEVEL LoggingLevel::DEBUG // Can be DEBUG, INFO, WARNING, ERROR or CRITICAL
#endif

// Restart the esp at this time
#define TARGET_TIME_FOR_ESP_RESTART 12600000U // [ms] = (3 * 3600 + 30 * 60) * 1000 -> 03:30 AM

// Check for firmware update on boot
#define CHECK_FOR_FIRMWARE_UPDATE_ON_BOOT true

#define USB_SERIAL_BAUD 921600U

#define RECORD_GPS_WHILE_STANDING true
#define GPS_UPDATE_INTERVAL_WHILE_DRIVING 1000 // ms
#define GPS_UPDATE_INTERVAL_WHILE_STANDING 2000 // ms

// Hardware watchdog
#define HW_WATCHDOG_INITIAL_STARTUP_TIMEOUT 1200U // [s]
#define HW_WATCHDOG_DEFAULT_TIMEOUT 300U // [s] If the watchdog doesn't get reset in this time, it will restart the esp

// Connection speed test
#define GIVE_CONNECTION_SPEED_ESTIMATE false
#define CONNECTION_SPEED_TEST_FILE_SIZE (32 * 1024) // Smaller files result in less accurate connection speed estimates.
