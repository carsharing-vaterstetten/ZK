#pragma once
#include <cstdint>
#include <esp_err.h>

class WatchdogHandler
{
    uint32_t currentTimeout = UINT32_MAX;
    bool isSubscribedToTask = false;
public:
    esp_err_t setTimeout(uint32_t timeout);
    esp_err_t resetTimeout();
    static esp_err_t taskWDTReset();
    esp_err_t subscribeTask();
    [[nodiscard]] uint32_t getCurrentTimeout() const;
};
