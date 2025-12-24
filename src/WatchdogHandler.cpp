#include "WatchdogHandler.h"

#include <esp_task_wdt.h>

#include "Globals.h"

uint32_t WatchdogHandler::getCurrentTimeout() const
{
    return currentTimeout;
}

/// This function configures and initializes the TWDT. If the TWDT is already initialized when this function is called, this function will update the TWDT's timeout period
/// @param timeout Timeout period of TWDT in seconds
esp_err_t WatchdogHandler::setTimeout(const uint32_t timeout)
{
    esp_task_wdt_reset(); // Reset watchdog to ensure it does not immediately trigger when setting to a lower timeout
    const esp_err_t watchdog_init_err = esp_task_wdt_init(timeout, true);
    fileLog.logInfoOrErrorln(watchdog_init_err == ESP_OK, "TWDT timeout set successfully to " + String(timeout) + " s",
                             "Setting TWDT timeout to " + String(timeout) + " s failed. Error " + String(
                                 watchdog_init_err));
    if (watchdog_init_err == ESP_OK)
    {
        currentTimeout = timeout;
    }
    return watchdog_init_err;
}

esp_err_t WatchdogHandler::resetTimeout()
{
    return setTimeout(HW_WATCHDOG_DEFAULT_TIMEOUT);
}

esp_err_t WatchdogHandler::taskWDTReset()
{
    return esp_task_wdt_reset();
}

esp_err_t WatchdogHandler::subscribeTask()
{
    const esp_err_t task_wdt_add_error = esp_task_wdt_add(nullptr);
    isSubscribedToTask = task_wdt_add_error == ESP_OK;
    fileLog.logInfoOrErrorln(isSubscribedToTask, "Successfully subscribed current task to TWDT.",
                             "Failed to subscribe current task to TWDT. Error " + String(task_wdt_add_error));
    return task_wdt_add_error;
}
