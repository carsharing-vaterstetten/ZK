#include "Globals.h"

#include "Config.h"

Log serialOnlyLog{};
Log fileLog{};
String modemIMEI = "";
LED statusLed{LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800};
bool isLoggedIn = false;
uint32_t loggedInRFID = 0;
LocalConfig config{
    DEFAULT_CONFIG_APN,
    DEFAULT_CONFIG_GPRS_USER,
    DEFAULT_CONFIG_GPRS_PASSWORD,
    DEFAULT_CONFIG_SERVER,
    DEFAULT_CONFIG_PORT,
    DEFAULT_CONFIG_PASSWORD,
    DEFAULT_SIM_PIN
};
