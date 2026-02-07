#pragma once

#include <WString.h>

#include "Config.h"
#include "Modem.h"
#include "AccessControl.h"
#include "GPS.h"
#include "Api.h"
#include "Backend.h"
#include "Intern.h"
#include "LocalConfig.h"
#include "LED.h"
#include "Log.h"
#include "NFCCardReader.h"
#include "SwappableFile.h"

inline String modemIMEI = "";
inline Modem modem{Serial1, MODEM_SERIAL_BAUD, MODEM_RX_PIN, MODEM_TX_PIN};
inline AccessControl accessControl{OPEN_KEY, CLOSE_KEY, "AccCtrl v1"};
inline GPS gps{GPS_FILE_PATH, GPS_FILE_UPLOAD_ENDPOINT};
inline ApiClient api{512};
inline CardReaderLED statusLed{LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800};
inline auto config = new LocalConfig{
    DEFAULT_CONFIG_APN,
    DEFAULT_CONFIG_GPRS_USER,
    DEFAULT_CONFIG_GPRS_PASSWORD,
    DEFAULT_CONFIG_SERVER,
    DEFAULT_CONFIG_PORT,
    DEFAULT_CONFIG_PASSWORD,
    DEFAULT_SIM_PIN
};
inline Log serialOnlyLog{};
inline Log fileLog{};
inline SPIClass nfcSpi{NFC_SPI};
inline NFCCardReader cardReader{nfcSpi, NFC_SS};
inline WatchdogHandler watchdogHandler;
inline SwappableFile swLog{PRIMARY_LOG_FILE_PATH, SECONDARY_LOG_FILE_PATH};
