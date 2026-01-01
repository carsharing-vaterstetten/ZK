#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <cstdint>
#include <FS.h>
#include <optional>
#include <WString.h>

#include "Log.h"
#include "TinyGsmGPRS.tpp"

#define STR_HELPER(s) #s
#define STR(s) STR_HELPER(s)


class Log;
class LocalConfig;

namespace HelperUtils
{
    static constexpr size_t dateTimeStrLength = 32;

    std::optional<LocalConfig> parseConfigString(const String& inputString);
    LocalConfig requestConfig();
    bool md5File(File file, uint8_t out[16]);
    String md5ToHex(const uint8_t md5[16]);
    time_t dateTimeToUnixTimestamp(int year, int month, int day, int hour, int minute, int second,
                                   float timezone);
    void dateTimeToString(char* buf, int year, int month, int day, int hour, int minute, int second);
    uint64_t systemTimeMillisecondsSinceEpoche();
    bool isSuccessfulResponse(int statusCode);
    String simStatusToString(SimStatus status);
    String millisToIsoString(uint64_t ms);
    String getResetReasonHumanReadable(int reset_reason);
    String toBase64(const uint8_t* data, size_t len);
    void logRAMUsage(const Log& log, LoggingLevel level);
    void uploadLog(bool deleteIfSuccess, bool deleteAfterRetrying, size_t retries);
    void performConnectionSpeedTest(size_t fileSize);
    bool syncTimeWithModem(size_t maxRetries);
}
