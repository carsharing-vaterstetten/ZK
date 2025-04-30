#ifndef CONFIG_H
#define CONFIG_H

#include <Intern.h>

#define UART_BAUD 115200

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_T_PCIE
#define TINY_GSM_RX_BUFFER 1024 // 1Kb

#define LED_PIN 12
#define LED_COUNT 4

// Modem Pins
#define PIN_TX 27
#define PIN_RX 26
#define PWR_PIN 4
#define POWER_PIN 25

// Schlüssel knopfe
#define OPEN_KEY 15
#define CLOSE_KEY 14

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

//#define NCF_I2C

// NFC Modul Pins
#define NFC_MOSI 23
#define NFC_MISO 19
#define NFC_SCLK 18
#define NFC_SS 5

#define NCF_SDA 21
#define NCF_SCL 22

// ESP32 startet sich jeden tag um die Uhrzeit neu
const unsigned long targetTimeToRestartESP32 = (03 * 3600 + 30 * 60) * 1000; // 03:30 Uhr

#define HW_WATCHDOG_TIMEOUT (300) // [Sekunden] Maximal erlaubte Zeit ohne Reset – nach 5 Minuten startet der HW-Watchdog den Arduino neu
#define HW_WATCHDOG_RESET_DELAY_MS (100) // [Millisekunden] Der Arduino muss spätestens alle 100ms den Watchdog zurücksetzen, damit der 5-Minuten-Timeout nicht abläuft

#endif
