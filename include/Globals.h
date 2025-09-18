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
extern const Config config;
#else
extern Config config;
#endif
