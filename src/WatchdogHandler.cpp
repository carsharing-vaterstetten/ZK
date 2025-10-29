#include "WatchdogHandler.h"

#include <esp_task_wdt.h>

#include "Config.h"
#include "Globals.h"

uint32_t WatchdogHandler::currentTimeout = UINT32_MAX;
uint32_t WatchdogHandler::timeoutBeforeTempSet = UINT32_MAX;
bool WatchdogHandler::tempTimeoutSet = false;

uint32_t WatchdogHandler::getCurrentTimeout()
{
    return currentTimeout;
}

/// Can be used to increase the TWDT timeout and then later revert it to the timout it was before with revertTempSet
esp_err_t WatchdogHandler::increaseTimeoutTemporarily(const uint32_t timeout)
{
    const uint32_t oldTimeout = currentTimeout;
    const esp_err_t err = setTimeout(timeout + currentTimeout);
    if (err == ESP_OK)
    {
        timeoutBeforeTempSet = oldTimeout;
        tempTimeoutSet = true;
    }
    return err;
}

esp_err_t WatchdogHandler::revertTemporaryIncrease()
{
    if (!tempTimeoutSet) return ESP_OK;
    return setTimeout(timeoutBeforeTempSet);
}

/// This function configures and initializes the TWDT. If the TWDT is already initialized when this function is called, this function will update the TWDT's timeout period
/// @param timeout Timeout period of TWDT in seconds
esp_err_t WatchdogHandler::setTimeout(const uint32_t timeout)
{
    esp_task_wdt_reset(); // Reset watchdog to ensure it does not immediately trigger when setting to a lower timeout
    const esp_err_t watchdog_init_err = esp_task_wdt_init(timeout, true);
    fileLog.logInfoOrErrorln(watchdog_init_err == ESP_OK, "TWDT timeout set successfully to " + String(timeout) + " s",
                             "Setting TWDT timeout to " + String(timeout) + " s failed. Error " + String(watchdog_init_err));
    if (watchdog_init_err == ESP_OK)
    {
        currentTimeout = timeout;
        tempTimeoutSet = false;
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
    fileLog.logInfoOrErrorln(task_wdt_add_error == ESP_OK, "Successfully subscribed current task to TWDT.",
                             "Failed to subscribe current task to TWDT. Error " + String(task_wdt_add_error));
    return task_wdt_add_error;
}
