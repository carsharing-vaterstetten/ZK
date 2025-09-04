#pragma once


#include <Intern.h>
#include <FS.h>

class HelperUtils
{
public:
    static void parseConfigString(const String& inputString, Config& c);
    static String getConfigHumanReadable(const Config& c);
    static String getConfigFormat(const Config& c);
    static String getResetReasonHumanReadable(esp_reset_reason_t reset_reason);
    static esp_err_t setWatchdog(uint32_t watchdog_timeout);
    static esp_err_t subscribeTaskToWatchdog();
    static void requestConfig(Config& c);
    static bool md5File(File file, uint8_t out[16]);
    static String md5ToHex(const uint8_t md5[16]);
};
