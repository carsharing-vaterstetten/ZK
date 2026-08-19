#include "system/Watchdog.h"

#include <esp_task_wdt.h>

#include "config/user_config.h"
#include "logging/Loggers.h"

void Watchdog::begin()
{
    const esp_err_t err = esp_task_wdt_init(HW_WATCHDOG_INITIAL_STARTUP_TIMEOUT, true);
    logger.logInfoOrCriticalErrorln(err == ESP_OK,
        "Task watchdog started (" + String(HW_WATCHDOG_INITIAL_STARTUP_TIMEOUT) + "s startup timeout)",
        "Failed to start task watchdog");
}

void Watchdog::enterSteadyState()
{
    // esp_task_wdt_init() on an already-initialised TWDT just updates the
    // timeout rather than erroring, which is exactly the tighten-after-boot
    // behaviour wanted here.
    const esp_err_t err = esp_task_wdt_init(HW_WATCHDOG_DEFAULT_TIMEOUT, true);
    logger.logInfoOrErrorln(err == ESP_OK,
        "Task watchdog tightened to " + String(HW_WATCHDOG_DEFAULT_TIMEOUT) + "s steady-state timeout",
        "Failed to tighten task watchdog timeout");
}

void Watchdog::watchCurrentTask()
{
    esp_task_wdt_add(nullptr);
}

void Watchdog::unwatchCurrentTask()
{
    esp_task_wdt_delete(nullptr);
}

void Watchdog::feed()
{
    esp_task_wdt_reset();
}
