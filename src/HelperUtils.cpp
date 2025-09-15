// HelperUtils.cpp

#include "HelperUtils.h"
#include "Globals.h"
#include <EEPROM.h>
#include "mbedtls/md5.h"
#include <FS.h>

#include "Config.h"
#include "Modem.h"


void HelperUtils::parseConfigString(const String& inputString, Config& c)
{
    // String anhand von ';' aufteilen
    int start = 0;

    while (start < inputString.length())
    {
        int end = inputString.indexOf(';', start);

        if (end == -1)
        {
            end = inputString.length();
        }

        String token = inputString.substring(start, end);

        // Token verarbeiten, sollte in der Form key="value" oder key=value sein
        const int eqIndex = token.indexOf('=');

        if (eqIndex != -1)
        {
            String key = token.substring(0, eqIndex);
            String value = token.substring(eqIndex + 1);

            value.trim();

            if (value.startsWith("\"") && value.endsWith("\""))
            {
                value = value.substring(1, value.length() - 1);
            }

            key.trim();

            if (key == "apn")
            {
                value.toCharArray(c.apn, sizeof(c.apn));
            }
            else if (key == "server")
            {
                value.toCharArray(c.server, sizeof(c.server));
            }
            else if (key == "port")
            {
                c.port = value.toInt();
            }
            else if (key == "password")
            {
                value.toCharArray(c.password, sizeof(c.password));
            }
            else if (key == "preferSDCard")
            {
                c.preferSDCard = value.toInt();
            }
            else
            {
                fileLog.warningln("Encountered an unknown key while decoding user input config: '" + key + "'");
            }
        }
        start = end + 1;
    }
}

String HelperUtils::getConfigHumanReadable(const Config& c)
{
    return "Config version: " + String(c.version) + " apn=" + c.apn + " server=" + c.server + " port=" + String(c.port)
        + " password=" + c.password + " preferSDCard=" + String(c.preferSDCard);
}

String HelperUtils::getConfigHumanReadableHideSecrets(const Config& c)
{
    return "Config version: " + String(c.version) + " apn=" + c.apn + " server=" + c.server + " port=" + String(c.port)
        + " preferSDCard=" + String(c.preferSDCard);
}

String HelperUtils::getConfigFormat(const Config& c)
{
    return "apn=\"" + String(c.apn) + "\";server=\"" + c.server + "\";port=\"" + String(c.port) +
        +"\";password=\"" + c.password + "\";preferSDCard=\"" + String(c.preferSDCard) + "\";";
}

void HelperUtils::requestConfig(Config& c)
{
    Serial.println("Please enter config data in this format:");
    constexpr Config exampleConfig = {
        CONFIG_VERSION,
        "iot.1nce.net",
        "example.com",
        80,
        "XXX",
        true,
    };
    Serial.println(getConfigFormat(exampleConfig));

    String inputString = "";

    while (inputString.length() == 0)
    {
        if (Serial.available())
        {
            inputString = Serial.readStringUntil('\n');
        }
    }

    fileLog.infoln("User entered config String: " + inputString);

    parseConfigString(inputString, c);
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
