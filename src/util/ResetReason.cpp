#include "util/ResetReason.h"

String ResetReason::describe(const RESET_REASON reason)
{
    switch (reason)
    {
    case NO_MEAN: return "NO_MEAN";
    case POWERON_RESET: return "Vbat power on reset";
    case SW_RESET: return "Software reset digital core";
    case OWDT_RESET: return "Legacy watch dog reset digital core";
    case DEEPSLEEP_RESET: return "Deep Sleep reset digital core";
    case SDIO_RESET: return "Reset by SLC module, reset digital core";
    case TG0WDT_SYS_RESET: return "Timer Group0 Watch dog reset digital core";
    case TG1WDT_SYS_RESET: return "Timer Group1 Watch dog reset digital core";
    case RTCWDT_SYS_RESET: return "RTC Watch dog Reset digital core";
    case INTRUSION_RESET: return "Intrusion tested to reset CPU";
    case TGWDT_CPU_RESET: return "Time Group reset CPU";
    case SW_CPU_RESET: return "Software reset CPU";
    case RTCWDT_CPU_RESET: return "RTC Watch dog Reset CPU";
    case EXT_CPU_RESET: return "for APP CPU, reset by PRO CPU";
    case RTCWDT_BROWN_OUT_RESET: return "Reset when the vdd voltage is not stable";
    case RTCWDT_RTC_RESET: return "RTC Watch dog reset digital core and rtc module";
    default: return "Unknown";
    }
}

String ResetReason::describe(const esp_reset_reason_t reason)
{
    switch (reason)
    {
    case ESP_RST_UNKNOWN: return "Unknown";
    case ESP_RST_POWERON: return "Power-on";
    case ESP_RST_EXT: return "External pin";
    case ESP_RST_SW: return "Software (ESP.restart())";
    case ESP_RST_PANIC: return "Panic / exception";
    case ESP_RST_INT_WDT: return "Interrupt watchdog";
    case ESP_RST_TASK_WDT: return "Task watchdog";
    case ESP_RST_WDT: return "Other watchdog";
    case ESP_RST_DEEPSLEEP: return "Deep sleep wakeup";
    case ESP_RST_BROWNOUT: return "Brownout";
    case ESP_RST_SDIO: return "SDIO";
    default: return "Unknown";
    }
}
