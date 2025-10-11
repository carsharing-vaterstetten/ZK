#pragma once

#include "LED.h"
#include "LocalConfig.h"
#include "Log.h"

extern Log serialOnlyLog;
extern Log fileLog;
extern String modemIMEI;
extern LED statusLed;
extern bool isLoggedIn;
extern uint32_t loggedInRFID;
extern LocalConfig config;
