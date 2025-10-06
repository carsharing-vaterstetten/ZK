#pragma once

#include "Intern.h"
#include "LED.h"
#include "LocalConfig.h"
#include "Log.h"

extern Log serialOnlyLog;
extern Log fileLog;
extern uint64_t efuseMac;
extern String efuseMacHex;
extern LED statusLed;
extern bool isLoggedIn;
extern uint32_t loggedInRFID;
extern LocalConfig config;
