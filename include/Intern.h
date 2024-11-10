
#ifndef Intern_H
#define Intern_H

#define FIRMWARE_VERSION "0.4.2"
#define RFID_FILE_NAME "/rfids.txt"
#define FIRMWARE_FILE_NAME "/firmware.bin"
#define LOG_FILE_NAME "/logs.json"

extern String MAC_ADDRESS;

#define CONFIG_START_ADDRESS 0
#define CONFIG_VERSION 1

struct Config
{
  uint8_t version; // Wird verwendet, um eine Versionsnummer oder Signatur zu speichern
  char apn[32];
  char gprsUser[32];
  char gprsPass[32];
  char GSM_PIN[16];
  char server[64];
  int port;
  char username[32];
  char password[64];
};

#endif