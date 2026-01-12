#pragma once

#include <climits>
#include <esp_err.h>

class WatchdogHandler
{
    uint currentTimeout = UINT_MAX;
    bool isSubscribedToTask = false;
public:
    esp_err_t setTimeout(uint timeout);
    esp_err_t resetTimeout();
    static esp_err_t taskWDTReset();
    esp_err_t subscribeTask();
    [[nodiscard]] uint getCurrentTimeout() const;
};
