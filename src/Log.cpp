#include "Log.h"

#include <LittleFS.h>

#include "Config.h"
#include "HelperUtils.h"
#include "Modem.h"

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define BACKGROUND_COLOR_RED "\033[41m"

void Log::enableSerialLogging(const uint8_t loggingLevel, const String& serialName)
{
    logToSerial = true;
    serialLoggingLevel = loggingLevel;
    serialLoggingName = serialName;
}

bool Log::enableFlashLogging(const String& flashLogFileName, const uint8_t loggingLevel)
{
    if (!LittleFS.exists(flashLogFileName))
    {
        // Let's ignore directories for now
        File file = LittleFS.open(flashLogFileName, FILE_WRITE, true);
        if (!file) return false;
        file.close();
    }

    flashLogPath = flashLogFileName;
    logToFlash = true;
    flashLoggingLevel = loggingLevel;
    return true;
}

void Log::stopSerialLogging()
{
    logToSerial = false;
}

void Log::stopFlashLogging()
{
    logToFlash = false;
}


void Log::logInfoOrLevelln(const bool success, const String& ifSuccess, const String& ifError,
                           const uint8_t level) const
{
    if (success)
    {
        infoln(ifSuccess);
    }
    else
    {
        logMsgln(ifError, level);
    }
}

void Log::logMsgln(const String& msg, const uint8_t level) const
{
    // = millis() if modem is not initialized
    const uint64_t timestampMs = HelperUtils::systemTimeMillisecondsSinceEpoche();

    if (logToSerial && level >= serialLoggingLevel)
        appendMsgToSerial(timestampMs, level, msg);

    if (logToFlash && level >= flashLoggingLevel)
        appendMsgToFile(timestampMs, level, msg);
}

void Log::appendMsgToSerial(const uint64_t timestamp, const uint8_t loggingLevel, const String& text) const
{
    Serial.print("[");
    Serial.print(HelperUtils::millisToIsoString(timestamp));
    Serial.print("][");
#if COLORIZE_SERIAL_LOGGING
    Serial.print(getLoggingLevelColor(loggingLevel));
#endif
    Serial.print(getLoggingLevelChar(loggingLevel));
#if COLORIZE_SERIAL_LOGGING
    Serial.print(COLOR_RESET);
#endif
    Serial.print("]");

    if (serialLoggingName.isEmpty())
    {
        Serial.print(" ");
    }
    else
    {
        Serial.print("[");
        Serial.print(serialLoggingName);
        Serial.print("] ");
    }

#if COLORIZE_SERIAL_LOGGING
    if (loggingLevel >= LOGGING_LEVEL_ERROR)
        Serial.print(BACKGROUND_COLOR_RED);
#endif

    Serial.print(text);

#if COLORIZE_SERIAL_LOGGING
    if (loggingLevel >= LOGGING_LEVEL_ERROR)
        Serial.print(COLOR_RESET);
#endif

    Serial.println();
}

void Log::appendMsgToFile(const uint64_t timestamp, const uint8_t loggingLevel, const String& text) const
{
    File file = LittleFS.open(flashLogPath, FILE_APPEND);
    if (!file) return;

    file.print("[");
    file.print(HelperUtils::millisToIsoString(timestamp));
    file.print("][");
    file.print(getLoggingLevelChar(loggingLevel));
    file.print("] ");
    file.println(text);

    file.close();
}

String Log::getLoggingLevelChar(const uint8_t level)
{
    switch (level)
    {
    case LOGGING_LEVEL_DEBUG:
        return "D";
    case LOGGING_LEVEL_INFO:
        return "I";
    case LOGGING_LEVEL_WARNING:
        return "W";
    case LOGGING_LEVEL_ERROR:
        return "E";
    case LOGGING_LEVEL_CRITICAL:
        return "C";
    default:
        throw std::invalid_argument("Invalid logging level");
    }
}

String Log::getLoggingLevelColor(const uint8_t level)
{
    switch (level)
    {
    case LOGGING_LEVEL_DEBUG:
        return COLOR_GREEN;
    case LOGGING_LEVEL_INFO:
        return COLOR_BLUE;
    case LOGGING_LEVEL_WARNING:
        return COLOR_YELLOW;
    case LOGGING_LEVEL_ERROR:
        return COLOR_RED;
    case LOGGING_LEVEL_CRITICAL:
        return COLOR_MAGENTA;
    default:
        throw std::invalid_argument("Invalid logging level");
    }
}
