#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <cstdint>
#include <FS.h>
#include <optional>
#include <WString.h>
#include <rom/rtc.h>

#include "net/ApiClient.h"
#include "logging/Log.h"
#include "TinyGsmGPRS.tpp"

#define STR_HELPER(s) #s
#define STR(s) STR_HELPER(s)


class SwappableFile;
class Log;
class LocalConfig;

namespace HelperUtils
{
    std::optional<LocalConfig> parseConfigString(const String& inputString);
    LocalConfig requestConfig();
    bool md5File(File file, uint8_t out[16]);
    time_t dateTimeToUnixTimestamp(int year, int month, int day, int hour, int minute, int second,
                                   float timezone);
    uint64_t systemTimeMillisecondsSinceEpoche();
    String millisToIsoString(uint64_t ms);
    String getResetReasonHumanReadable(RESET_REASON reset_reason);
    String toBase64(const uint8_t* data, size_t len);
    void uploadLog(ApiClient& api, SwappableFile& swLog, bool deleteIfSuccess, bool deleteAfterRetrying, uint retries);
    void uploadLogAndDeleteAfterRetryingIfLogIsTooLarge(ApiClient& api,
                                                        SwappableFile& swLog, uint retries = 2, bool deleteIfSuccess = true);
    void performConnectionSpeedTest(ApiClient& api, size_t fileSize);
    void syncSystemTime(time_t unixTimestamp);
}
