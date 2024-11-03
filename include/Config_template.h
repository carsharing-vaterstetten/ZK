#ifndef CONFIG_H
#define CONFIG_H

#include <Intern.h>

#define UART_BAUD 115200

#define TINY_GSM_MODEM_SIM7000SSL
#define TINY_GSM_RX_BUFFER 1024 // 1Kb

// LED Pin
#define LED_PIN 12
// Anzahl die LEDs
#define LED_COUNT 3

// Schlüssel knopfe
#define OPEN_KEY 15
#define CLOSE_KEY 14

// Modem Pins
#define PIN_TX 27
#define PIN_RX 26
#define PWR_PIN 4

/*
 *   2 Automatic
 *   13 GSM only
 *   38 LTE only
 *   51 GSM and LTE only
 */
extern byte NETWORK_MODE;

/*
 *    1 CAT-M
 *    2 NB-Iot
 *    3 CAT-M and NB-IoT
 */
extern byte PREFERRED_MODE;

// GPRS und apn Daten
const char apn[] = ""; // Zb: iot.1nce.net
const char gprsUser[] = "";
const char gprsPass[] = "";

// Sim pin
#define GSM_PIN ""

// NFC Modul Pins
#define NFC_MOSI 23
#define NFC_MISO 19
#define NFC_SCLK 18
#define NFC_SS 5

// Server Daten für abholen der RFID's usw.
const char server[] = "";
const int port = 443;
const char username[] = "";
const char password[] = "";

// ESP32 startet sich jeden tag um die Uhrzeit neu
const unsigned long targetTimeToRestartESP32 = (19 * 3600 + 30 * 60) * 1000; // 19:30 Uhr

#endif
