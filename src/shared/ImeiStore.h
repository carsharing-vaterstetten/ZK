#pragma once
#include <mutex>
#include <optional>
#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class ImeiStore
{
public:
    ImeiStore()
    {
        imeiReadySemaphore = xSemaphoreCreateBinary();
    }

    ~ImeiStore()
    {
        if (imeiReadySemaphore != nullptr)
            vSemaphoreDelete(imeiReadySemaphore);
    }

    std::optional<String> getIMEI() const
    {
        std::lock_guard lock(mtx);
        return imei;
    }

    void setIMEI(const String& newImei)
    {
        {
            std::lock_guard lock(mtx);
            imei = newImei;
        }

        if (imeiReadySemaphore != nullptr)
            xSemaphoreGive(imeiReadySemaphore);
    }

    String waitForIMEI() const
    {
        std::optional<String> gotImei = getIMEI();
        if (gotImei.has_value())
        {
            return gotImei.value();
        }

        if (imeiReadySemaphore != nullptr)
            xSemaphoreTake(imeiReadySemaphore, portMAX_DELAY);

        std::lock_guard lock(mtx);
        return imei.value_or("");
    }

private:
    mutable std::mutex mtx;
    std::optional<String> imei = std::nullopt;

    SemaphoreHandle_t imeiReadySemaphore = nullptr;
};
