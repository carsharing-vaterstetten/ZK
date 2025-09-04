#include "Globals.h"

#include "Config.h"

Log serialOnlyLog{true, true, "Serial"};
Log fileLog{true, true};
uint64_t efuseMac = 0;
LED statusLed{LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800};
bool isLoggedIn = false;
uint32_t loggedInRFID = 0;
NFC cardReader;
Config config;
