#include "Platform.h"

#include <esp32-hal.h>
#include <esp_log.h>
#include <esp_task_wdt.h>
#include <LittleFS.h>

#include "config/Config.h"
#include "config/Intern.h"
#include "config/user_config.h"
#include "logging/Loggers.h"
#include "util/ResetReason.h"

namespace
{
    /// esp_log output is routed here so IDF-level failures land in the same file
    /// as everything else. Log::logMsgln drops the line if this re-enters from
    /// inside its own write path.
    int espLogHandler(const char* fmt, const va_list args)
    {
        char buf[256];
        vsnprintf(buf, sizeof(buf), fmt, args);
        logger.errorln(buf);
        return 0;
    }
}

void Platform::begin()
{
    disableCore0WDT();
    disableCore1WDT();
    disableLoopWDT();
    esp_task_wdt_deinit();

    Serial.begin(USB_SERIAL_BAUD);
    while (!Serial) {}

    if constexpr (ENABLE_SERIAL_LOGGING)
    {
        logger.addOutputSink(Serial, "", true, COLORIZE_SERIAL_LOGGING, SERIAL_LOGGING_LEVEL);
        serialLogger.addOutputSink(Serial, "Serial", true, COLORIZE_SERIAL_LOGGING, SERIAL_LOGGING_LEVEL);
    }

    const bool flashInitSuccess = LittleFS.begin(true);
    serialLogger.logInfoOrCriticalErrorln(flashInitSuccess, "Flash initialized successfully",
                                          "Flash initialization failed");

    LittleFS.remove("/gps.bin"); // Delete old file

    swLog.begin(true);
    logger.addOutputSink(swLog, "", true, false, FLASH_LOGGING_LEVEL, true, true);

    // Logging to flash is possible from here on.
    esp_log_set_vprintf(&espLogHandler);

    logger.infoln("Running firmware version " FIRMWARE_VERSION);
    logger.infoln("CPU0 reset reason: " + ResetReason::describe(rtc_get_reset_reason(0)));
    logger.infoln("CPU1 reset reason: " + ResetReason::describe(rtc_get_reset_reason(1)));
}

const BoardConfig& Platform::selectBoard()
{
#ifdef HW_REV
    return *activeBoard<HW_REV>();
#else
    Serial.println("No hardware revision specified. Please enter revision number:");
    Serial.setTimeout(100000);

    while (true)
    {
        if (!Serial.available()) continue;

        const int rev = Serial.readStringUntil('\n').toInt();

        if (const BoardConfig* board = getBoard(rev))
        {
            Serial.println("Using board revision " + String(rev));
            return *board;
        }

        Serial.println("Unknown revision, try again:");
    }
#endif
}

LocalConfig Platform::loadConfig()
{
    if constexpr (USE_DEFAULT_CONFIG)
    {
        logger.infoln("Using default config");
        return LocalConfig{
            DEFAULT_CONFIG_APN, DEFAULT_CONFIG_GPRS_USER, DEFAULT_CONFIG_GPRS_PASSWORD,
            DEFAULT_CONFIG_SERVER, DEFAULT_CONFIG_PORT, DEFAULT_CONFIG_PASSWORD, DEFAULT_SIM_PIN
        };
    }

    if (std::optional<LocalConfig> stored = LocalConfig::fromStorage(CONFIG_PREFS_NAME))
        return stored.value();

    serialLogger.warningln("No or outdated config found. Requesting new config.");

    LocalConfig entered = LocalConfig::promptOverSerial();
    const bool saved = StorableConfig{entered, CONFIG_PREFS_NAME}.save();
    logger.logInfoOrErrorln(saved, "Successfully saved config", "Failed to save config");

    return entered;
}
