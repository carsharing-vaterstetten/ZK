#pragma once
#include <Arduino.h>

class WatchdogHandler
{
    static uint32_t currentTimeout;
    static uint32_t timeoutBeforeTempSet;
    static bool tempTimeoutSet;
public:
    static esp_err_t setTimeout(uint32_t timeout);
    static esp_err_t resetTimeout();
    static esp_err_t taskWDTReset();
    static esp_err_t subscribeTask();
    static uint32_t getCurrentTimeout();

    static esp_err_t increaseTimeoutTemporarily(uint32_t timeout);
    static esp_err_t revertTemporaryIncrease();
};
