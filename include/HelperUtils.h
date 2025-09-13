#pragma once


#include <Intern.h>
#include <FS.h>

class HelperUtils
{
public:
    static void parseConfigString(const String& inputString, Config& c);
    static String getConfigHumanReadable(const Config& c);
    static String getConfigFormat(const Config& c);
    static void requestConfig(Config& c);
    static bool md5File(File file, uint8_t out[16]);
    static String md5ToHex(const uint8_t md5[16]);
};
