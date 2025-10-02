#pragma once


#include <Intern.h>
#include <FS.h>
#include <sd_defines.h>

class HelperUtils
{
public:
    static constexpr uint8_t dateTimeStrLength = 32;

    static bool parseConfigString(const String& inputString, Config& c);
    static String getConfigHumanReadable(const Config& c);
    static String getConfigHumanReadableHideSecrets(const Config& c);
    static String getConfigFormat(const Config& c);
    static void requestConfig(Config& c);
    static bool md5File(File file, uint8_t out[16]);
    static String md5ToHex(const uint8_t md5[16]);
    static time_t dateTimeToUnixTimestamp(int year, int month, int day, int hour, int minute, int second,
                                          float timezone);
    static void dateTimeToString(char* buf, int year, int month, int day, int hour, int minute, int second);
    static bool updateSystemTimeWithModem();
    static uint64_t systemTimeMillisecondsSinceEpoche();
    static const char* sdCardTypeName(sdcard_type_t type);
    static bool isSuccessfulResponse(int statusCode);
};
