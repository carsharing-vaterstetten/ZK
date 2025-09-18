#include "Globals.h"

#include "Config.h"
#include "Intern.h"

Log serialOnlyLog{};
Log fileLog{};
uint64_t efuseMac = 0;
String efuseMacHex = "";
LED statusLed{LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800};
bool isLoggedIn = false;
bool currentRFIDConsentsToGPSTracking = false;
uint32_t loggedInRFID = 0;
#if OVERRIDE_CONFIG
const Config config {
    CONFIG_VERSION,
    OVERRIDE_CONFIG_APN,
     OVERRIDE_CONFIG_SERVER,
     OVERRIDE_CONFIG_PORT,
     OVERRIDE_CONFIG_PASSWORD,
     OVERRIDE_CONFIG_PREFER_SD_CARD,
};
#else
Config config;
#endif
