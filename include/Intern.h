#pragma once

#include <Arduino.h>

#define FIRMWARE_VERSION "0.6.0"

// Local paths
#define RFID_FILE_PATH "/rfids.bin"
#define TMP_RFID_FILE_PATH "/tmp_rfids.bin"
#define FIRMWARE_FILE_PATH "/firmware.bin"
#define LOG_FILE_PATH "/log.log"

#define CONFIG_START_ADDRESS 0
#define CONFIG_VERSION 2

struct Config
{
  uint8_t version; // Wird verwendet, um eine Versionsnummer oder Signatur zu speichern
  char apn[32];
  char gprsUser[32];
  char gprsPass[32];
  char GSM_PIN[16];
  char server[64];
  uint16_t port;
  char username[32];
  char password[64];
  bool preferSDCard;
};
