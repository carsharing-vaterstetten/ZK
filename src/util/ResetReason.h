#pragma once

#include <esp_system.h>
#include <rom/rtc.h>
#include <WString.h>

namespace ResetReason
{
    String describe(RESET_REASON reason);

    /// The coarser, per-core RESET_REASON above can't tell a crash apart from a
    /// normal ESP.restart(): both report SW_CPU_RESET. esp_reset_reason() is a
    /// single, chip-wide reading from the same underlying reset-cause hardware,
    /// but with ESP_RST_PANIC/ESP_RST_TASK_WDT/ESP_RST_INT_WDT broken out as
    /// their own values — log both, this one is what actually answers "did it
    /// crash".
    String describe(esp_reset_reason_t reason);
}
