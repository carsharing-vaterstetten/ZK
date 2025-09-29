#pragma once

#include <Arduino.h>
#include "Config.h"

#define FIRMWARE_VERSION "1.0.0"

// Local paths
#define RFID_FILE_PATH "/rfids.bin"
#define TMP_RFID_FILE_PATH "/tmp_rfids.bin"
#define FIRMWARE_FILE_PATH "/firmware.bin"
#define LOG_FILE_PATH "/log.log"
#define CONNECTION_SPEED_TEST_FILE_PATH "/speed_test.bin"

#define CONFIG_START_ADDRESS 0

#if OVERRIDE_CONFIG
#define CONFIG_VERSION OVERRIDE_CONFIG_VERSION
#else
#define CONFIG_VERSION 2U
#endif

struct Config
{
  uint8_t version; // Wird verwendet, um eine Versionsnummer oder Signatur zu speichern
  char apn[32];
  char server[64];
  uint16_t port;
  char password[64];
  bool preferSDCard;
};
