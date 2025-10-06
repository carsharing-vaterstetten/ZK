#pragma once

#include <FS.h>

#define STR_HELPER(s) #s
#define STR(s) STR_HELPER(s)

class LocalConfig;

class HelperUtils
{
public:
    static constexpr uint8_t dateTimeStrLength = 32;

    static std::optional<LocalConfig> parseConfigString(const String& inputString);
    static LocalConfig requestConfig();
    static bool md5File(File file, uint8_t out[16]);
    static String md5ToHex(const uint8_t md5[16]);
    static time_t dateTimeToUnixTimestamp(int year, int month, int day, int hour, int minute, int second,
                                          float timezone);
    static void dateTimeToString(char* buf, int year, int month, int day, int hour, int minute, int second);
    static bool updateSystemTimeWithModem();
    static uint64_t systemTimeMillisecondsSinceEpoche();
    static bool isSuccessfulResponse(int statusCode);
};
