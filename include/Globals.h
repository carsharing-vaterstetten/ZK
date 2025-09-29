#pragma once

#include "Intern.h"
#include "LED.h"
#include "Log.h"

extern Log serialOnlyLog;
extern Log fileLog;
extern uint64_t efuseMac;
extern String efuseMacHex;
extern LED statusLed;
extern bool isLoggedIn;
extern bool currentRFIDConsentsToGPSTracking;
extern uint32_t loggedInRFID;

#if OVERRIDE_CONFIG
constexpr Config config {
    CONFIG_VERSION,
    OVERRIDE_CONFIG_APN,
     OVERRIDE_CONFIG_SERVER,
     OVERRIDE_CONFIG_PORT,
     OVERRIDE_CONFIG_PASSWORD,
     OVERRIDE_CONFIG_PREFER_SD_CARD,
};
#else
extern Config config;
#endif
