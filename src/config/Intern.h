#pragma once

#ifdef HW_REV
#if HW_REV == 3
#define FIRMWARE_VERSION_FLAG "hw3"
#elif HW_REV == 2
#define FIRMWARE_VERSION_FLAG "hw2"
#else
#error "Invalid HW_REV"
#endif
#else
#define FIRMWARE_VERSION_FLAG "main"
#endif

#define FIRMWARE_VERSION "2.0.1-" FIRMWARE_VERSION_FLAG

#define CONFIG_VERSION "1"
#define CONFIG_PREFS_NAME "Config v" CONFIG_VERSION

#define MODEM_SERIAL_BAUD 230400U
