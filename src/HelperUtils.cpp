// HelperUtils.cpp

#include "HelperUtils.h"
#include "Globals.h"
#include <EEPROM.h>
#include "mbedtls/md5.h"
#include <FS.h>

#include "Modem.h"


bool HelperUtils::parseConfigString(const String& inputString, Config& c)
{
    int start = 0;
    bool success = true;

    bool hasApn = false, hasServer = false, hasPort = false, hasPassword = false;

    while (start < inputString.length())
    {
        int end = inputString.indexOf(';', start);
        if (end == -1) end = inputString.length();

        String token = inputString.substring(start, end);
        token.trim();

        if (token.isEmpty())
        {
            start = end + 1;
            continue;
        }

        int eqIndex = token.indexOf('=');
        if (eqIndex == -1)
        {
            fileLog.warningln("Invalid config token (missing '='): '" + token + "'");
            success = false;
            start = end + 1;
            continue;
        }

        String key = token.substring(0, eqIndex);
        String value = token.substring(eqIndex + 1);

        key.trim();
        value.trim();

        if (value.length() >= 2 && value.startsWith("\"") && value.endsWith("\""))
        {
            value = value.substring(1, value.length() - 1);
        }

        if (key == "apn")
        {
            value.toCharArray(c.apn, sizeof(c.apn));
            hasApn = true;
        }
        else if (key == "server")
        {
            value.toCharArray(c.server, sizeof(c.server));
            hasServer = true;
        }
        else if (key == "port")
        {
            if (value.length() == 0)
            {
                fileLog.warningln("Invalid port value, using default");
                success = false;
            }
            else
            {
                c.port = value.toInt();
                hasPort = true;
            }
        }
        else if (key == "password")
        {
            value.toCharArray(c.password, sizeof(c.password));
            hasPassword = true;
        }
        else
        {
            fileLog.warningln("Unknown config key: '" + key + "'");
            success = false;
        }

        start = end + 1;
    }

    // Make sure all required keys were present
    if (!hasApn || !hasServer || !hasPort || !hasPassword)
    {
        fileLog.warningln("Missing required config keys!");
        success = false;
    }

    return success;
}


String HelperUtils::getConfigHumanReadable(const Config& c)
{
    return "Config version: " + String(c.version) + " apn=" + c.apn + " server=" + c.server + " port=" + String(c.port)
        + " password=" + c.password;
}

String HelperUtils::getConfigHumanReadableHideSecrets(const Config& c)
{
    return "Config version: " + String(c.version) + " apn=" + c.apn + " server=" + c.server + " port=" + String(c.port);
}

String HelperUtils::getConfigFormat(const Config& c)
{
    return "apn=\"" + String(c.apn) + "\";server=\"" + c.server + "\";port=\"" + String(c.port) +
        +"\";password=\"" + c.password + "\";";
}

void HelperUtils::requestConfig(Config& c)
{
    constexpr Config exampleConfig = {
        CONFIG_VERSION,
        "iot.1nce.net",
        "example.com",
        80,
        "XXX",
    };
    const String exampleConfigFormat = getConfigFormat(exampleConfig);

    String inputString = "";

    const unsigned long oldTimeout = Serial.getTimeout();
    Serial.setTimeout(100000000ULL);

    while (true)
    {
        Serial.println("Please enter config data in this format:");
        Serial.println(exampleConfigFormat);

        while (!Serial.available())
        {
        }

        inputString = Serial.readStringUntil('\n');
        inputString.trim();

        if (inputString.isEmpty())
        {
            Serial.println("Entered config is empty");
            continue;
        }

        Serial.println("Entered config string: " + inputString);

        const bool parseSuccess = parseConfigString(inputString, c);

        if (!parseSuccess)
        {
            Serial.println("Failed to parse config. Try again");
            continue;
        }

        break;
    }

    Serial.setTimeout(oldTimeout);
}

bool HelperUtils::md5File(File file, uint8_t out[16])
{
    mbedtls_md5_context ctx;
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts_ret(&ctx);

    uint8_t buf[512];
    while (file.available())
    {
        const size_t len = file.read(buf, sizeof(buf));
        mbedtls_md5_update_ret(&ctx, buf, len);
    }

    mbedtls_md5_finish_ret(&ctx, out);
    mbedtls_md5_free(&ctx);
    file.close();
    return true;
}

String HelperUtils::md5ToHex(const uint8_t md5[16])
{
    String hex;
    for (int i = 0; i < 16; i++)
    {
        char buf[3];
        sprintf(buf, "%02x", md5[i]);
        hex += buf;
    }
    return hex;
}

time_t HelperUtils::dateTimeToUnixTimestamp(const int year, const int month, const int day, const int hour,
                                            const int minute, const int second, const float timezone)
{
    tm datetime{};

    datetime.tm_year = year - 1900; // Number of years since 1900
    datetime.tm_mon = month - 1; // Number of months since January
    datetime.tm_mday = day;
    datetime.tm_hour = hour;
    datetime.tm_min = minute;
    datetime.tm_sec = second;
    // Daylight Savings must be specified
    // -1 uses the computer's timezone setting
    datetime.tm_isdst = -1;

    time_t time = mktime(&datetime);

    time -= static_cast<time_t>(timezone * 3600.0f);

    return time;
}

void HelperUtils::dateTimeToString(char* buf, const int year, const int month, const int day, const int hour,
                                   const int minute, const int second)
{
    snprintf(buf, dateTimeStrLength, "%04d-%02d-%02d %02d:%02d:%02d",
             year, month, day, hour, minute, second);
}

bool HelperUtils::updateSystemTimeWithModem()
{
    if (!Modem::isInitialized())
        return false;

    const time_t seconds = Modem::getUnixTimestamp();
    const timeval now = {.tv_sec = seconds, .tv_usec = 0};
    settimeofday(&now, nullptr);

    return true;
}

/// Same as millis() if system time is not initialized
uint64_t HelperUtils::systemTimeMillisecondsSinceEpoche()
{
    static timeval now{};
    gettimeofday(&now, nullptr);
    return now.tv_sec * 1000ULL + now.tv_usec / 1000ULL;
}

bool HelperUtils::isSuccessfulResponse(const int statusCode)
{
    return statusCode < 300 && statusCode >= 200;
}
