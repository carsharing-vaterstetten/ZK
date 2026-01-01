#pragma once

#include <cstdint>
#include <esp_err.h>

class WatchdogHandler
{
    size_t currentTimeout = SIZE_MAX;
    bool isSubscribedToTask = false;
public:
    esp_err_t setTimeout(uint32_t timeout);
    esp_err_t resetTimeout();
    static esp_err_t taskWDTReset();
    esp_err_t subscribeTask();
    [[nodiscard]] size_t getCurrentTimeout() const;
};
