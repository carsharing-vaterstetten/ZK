#pragma once

#include "Intern.h"
#include "LED.h"
#include "Log.h"
#include "NFC.h"

extern Log serialOnlyLog;
extern Log fileLog;
extern uint64_t efuseMac;
extern LED statusLed;
extern bool isLoggedIn;
extern uint32_t loggedInRFID;
extern NFC cardReader;
extern Config config;
