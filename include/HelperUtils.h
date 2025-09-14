#pragma once


#include <Intern.h>
#include <FS.h>

class HelperUtils
{
public:
    static constexpr uint8_t dateTimeStrLength = 32;

    static void parseConfigString(const String& inputString, Config& c);
    static String getConfigHumanReadable(const Config& c);
    static String getConfigFormat(const Config& c);
    static void requestConfig(Config& c);
    static bool md5File(File file, uint8_t out[16]);
    static String md5ToHex(const uint8_t md5[16]);
    static uint64_t dateTimeToUnixTimestamp(int year, int month, int day, int hour, int minute, int second);
    static void dateTimeToString(char* buf, int year, int month, int day, int hour, int minute, int second);
};
