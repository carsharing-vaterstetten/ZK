// HelperUtils.cpp

#include "HelperUtils.h"
#include "mbedtls/md5.h"
#include <iomanip>
#include <LittleFS.h>

#include "Globals.h"
#include "mbedtls/base64.h"

#include "Modem.h"
#include "LocalConfig.h"


std::optional<LocalConfig> HelperUtils::parseConfigString(const String& inputString)
{
    int start = 0;

    std::optional<String> apn, gprsUser, gprsPassword, server, serverPassword, simPin;
    std::optional<uint16_t> serverPort;

    while (start < inputString.length())
    {
        int end = inputString.indexOf(';', start);
        if (end == -1) end = static_cast<int>(inputString.length());

        String token = inputString.substring(start, end);
        token.trim();

        if (token.isEmpty())
        {
            start = end + 1;
            continue;
        }

        const int eqIndex = token.indexOf('=');
        if (eqIndex == -1)
        {
            fileLog.warningln("Invalid config token (missing '='): '" + token + "'");
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

        if (key == LocalConfig::apnKey)
            apn = value;
        else if (key == LocalConfig::gprsUserKey)
            gprsUser = value;
        else if (key == LocalConfig::gprsPasswordKey)
            gprsPassword = value;
        else if (key == LocalConfig::serverKey)
            server = value;
        else if (key == LocalConfig::serverPortKey)
            serverPort = value.toInt();
        else if (key == LocalConfig::serverPasswordKey)
            serverPassword = value;
        else if (key == LocalConfig::simPinKey)
            simPin = value;
        else
            fileLog.warningln("Unknown config key: '" + key + "'");

        start = end + 1;
    }

    if (!apn || !server || !serverPort || !serverPassword || !gprsUser || !gprsPassword || !simPin) return std::nullopt;

    return LocalConfig{
        apn.value(), gprsUser.value(), gprsPassword.value(), server.value(), serverPort.value(), serverPassword.value(),
        simPin.value()
    };
}

LocalConfig HelperUtils::requestConfig()
{
    const LocalConfig exampleConfig{
        "iot.1nce.net",
        "hans",
        "PWD",
        "example.com",
        80,
        "XXX",
        "1234"
    };
    const String exampleConfigFormat = exampleConfig.toString();

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

        if (const auto pc = parseConfigString(inputString))
        {
            Serial.println("Successfully parsed config: " + pc.value().toString());
            Serial.setTimeout(oldTimeout);
            return pc.value();
        }

        Serial.println("Failed to parse config. Try again");
    }
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

String HelperUtils::simStatusToString(const SimStatus status)
{
    switch (status)
    {
    case SIM_ERROR:
        return "ERROR";
    case SIM_READY:
        return "READY";
    case SIM_LOCKED:
        return "LOCKED";
    case SIM_ANTITHEFT_LOCKED:
        return "ANTITHEFT LOCKED";
    }

    return "UNKNOWN";
}

String HelperUtils::millisToIsoString(const uint64_t ms)
{
    const auto seconds = static_cast<time_t>(ms / 1000ULL); // convert to seconds
    tm timeinfo{};
    gmtime_r(&seconds, &timeinfo); // use UTC time

    char buf[30];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &timeinfo);

    // Add milliseconds
    char result[40];
    snprintf(result, sizeof(result), "%s.%03uZ", buf, static_cast<unsigned>(ms % 1000));
    return {result};
}

String HelperUtils::getResetReasonHumanReadable(const int reset_reason)
{
    switch (reset_reason)
    {
    case 1: return "Vbat power on reset";
    case 3: return "Software reset digital core";
    case 4: return "Legacy watch dog reset digital core";
    case 5: return "Deep Sleep reset digital core";
    case 6: return "Reset by SLC module, reset digital core";
    case 7: return "Timer Group0 Watch dog reset digital core";
    case 8: return "Timer Group1 Watch dog reset digital core";
    case 9: return "RTC Watch dog Reset digital core";
    case 10: return "Instrusion tested to reset CPU";
    case 11: return "Time Group reset CPU";
    case 12: return "Software reset CPU";
    case 13: return "RTC Watch dog Reset CPU";
    case 14: return "for APP CPU, reset by PRO CPU";
    case 15: return "Reset when the vdd voltage is not stable";
    case 16: return "RTC Watch dog reset digital core and rtc module";
    default: return "NO_MEAN";
    }
}

String HelperUtils::toBase64(const uint8_t* data, const size_t len)
{
    unsigned char encoded[64]; // plenty of space for 16-byte MD5
    size_t out_len = 0;

    mbedtls_base64_encode(encoded, sizeof(encoded), &out_len, data, len);
    encoded[out_len] = '\0'; // null-terminate

    return {reinterpret_cast<char*>(encoded)};
}

void HelperUtils::logRAMUsage(const Log& log, const LoggingLevel level)
{
    log.logMsgln(String("RAM Usage: Total Free: ") + esp_get_free_heap_size() + " B" +
                 " | Internal Free: " + heap_caps_get_free_size(MALLOC_CAP_INTERNAL) + " B" +
                 " | External Free: " + heap_caps_get_free_size(MALLOC_CAP_SPIRAM) + " B" +
                 " | Largest Internal Block: " + heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL) + " B",
                 level
    );
}

void HelperUtils::uploadLog(const bool deleteIfSuccess, const bool deleteAfterRetrying, const size_t retries)
{
    File f = LittleFS.open(LOG_FILE_PATH, FILE_READ);
    const size_t fileSize = f.size();
    f.close();
    fileLog.infoln("Uploading log file (" + String(fileSize) + " B)");
    Modem::uploadFileAndDelete(LOG_FILE_UPLOAD_ENDPOINT, LOG_FILE_PATH, deleteIfSuccess, deleteAfterRetrying, retries);
}

void HelperUtils::performConnectionSpeedTest(const size_t fileSize)
{
    fileLog.infoln("Performing connection speed test");

    const HttpRequest req = HttpRequest::post(CONNECTION_SPEED_TEST_ENDPOINT, randomStream, fileSize);
    const ApiResponse resp = api.makeRequest(req, true);

    if (!resp.valid)
    {
        fileLog.errorln("Request failed");
        return;
    }

    fileLog.infoln("Response code: " + String(resp.responseCode));

    if (resp.responseCode != 200)
    {
        fileLog.errorln("Unexpected status code");
        return;
    }

    const uint32_t estimatedUploadSpeed = fileSize * 1000 / resp.uploadTimeMs;
    fileLog.infoln("Upload test complete. Estimated speed: " + String(estimatedUploadSpeed) + " B/s");

    const uint64_t downloadStartMs = millis();
    api.fetch(resp, emptyStream);
    const size_t downloadTimeMs = millis() - downloadStartMs;

    const size_t estimatedDownloadSpeed = resp.bodyLength * 1000 / downloadTimeMs;
    fileLog.infoln("Download test complete. Estimated speed: " + String(estimatedDownloadSpeed) + " B/s");
}

bool HelperUtils::syncTimeWithModem(const size_t maxRetries)
{
    fileLog.infoln("Syncing time");
    size_t syncAttempt = 0;

    for (; syncAttempt <= maxRetries; ++syncAttempt)
    {
        int year;
        const bool getTimeSuccess = modem.getNetworkTime(&year, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
        fileLog.logInfoOrWarningln(getTimeSuccess, "Got time successfully", "Failed to get time");
        if (year < 2070 && year >= 2025) break;
        serialOnlyLog.warningln("Modem fetched nonsensical time (Year " + String(year) + ")");
    }

    const bool syncSuccess = syncAttempt < maxRetries;

    if (!syncSuccess) return false;

    const time_t seconds = modem.getUnixTimestamp();
    const timeval now = {.tv_sec = seconds, .tv_usec = 0};
    settimeofday(&now, nullptr);

    fileLog.logInfoOrWarningln(syncSuccess, "Time synced successfully", "Failed to sync time");

    return true;
}
