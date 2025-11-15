#include "Globals.h"

#include "Config.h"

Log serialOnlyLog{};
Log fileLog{};
String modemIMEI = "";
CardReaderLED statusLed{LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800};
LocalConfig config{
    DEFAULT_CONFIG_APN,
    DEFAULT_CONFIG_GPRS_USER,
    DEFAULT_CONFIG_GPRS_PASSWORD,
    DEFAULT_CONFIG_SERVER,
    DEFAULT_CONFIG_PORT,
    DEFAULT_CONFIG_PASSWORD,
    DEFAULT_SIM_PIN
};
