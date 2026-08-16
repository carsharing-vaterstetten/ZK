#pragma once

#define FLASH_LOGGING_LEVEL LoggingLevel::DEBUG

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
