#pragma once
#include <Arduino.h>

class WatchdogHandler
{
    uint32_t currentTimeout = UINT32_MAX;
    uint32_t timeoutBeforeTempSet = UINT32_MAX;
    bool tempTimeoutSet = false;
    bool isSubscribedToTask = false;
public:
    esp_err_t setTimeout(uint32_t timeout);
    esp_err_t resetTimeout();
    static esp_err_t taskWDTReset();
    esp_err_t subscribeTask();
    uint32_t getCurrentTimeout() const;

    esp_err_t increaseTimeoutTemporarily(uint32_t timeout);
    esp_err_t revertTemporaryIncrease();
};

inline WatchdogHandler watchdogHandler;
