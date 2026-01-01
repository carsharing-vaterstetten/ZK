#include "Log.h"

#include <LittleFS.h>

#include "HelperUtils.h"
#include "Modem.h"

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define BACKGROUND_COLOR_RED "\033[41m"

void Log::enableSerialLogging(const bool colorize, const LoggingLevel loggingLevel, const String& serialName)
{
    logToSerial = true;
    colorizeSerialLogging = colorize;
    serialLoggingLevel = loggingLevel;
    serialLoggingName = serialName;
}

bool Log::enableFlashLogging(const String& flashLogFileName, const LoggingLevel loggingLevel)
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
                           const LoggingLevel level) const
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

void Log::logMsgln(const String& msg, const LoggingLevel level) const
{
    // = millis() if modem is not initialized
    const uint64_t timestampMs = HelperUtils::systemTimeMillisecondsSinceEpoche();

    if (logToSerial && level >= serialLoggingLevel)
        appendMsgToSerial(timestampMs, level, msg);

    if (logToFlash && level >= flashLoggingLevel)
        appendMsgToFile(timestampMs, level, msg);
}

void Log::appendMsgToSerial(const uint64_t timestamp, const LoggingLevel loggingLevel, const String& text) const
{
    Serial.print("[");
    Serial.print(HelperUtils::millisToIsoString(timestamp));
    Serial.print("][");

    if (colorizeSerialLogging)
        Serial.print(getLoggingLevelColor(loggingLevel));

    Serial.print(getLoggingLevelChar(loggingLevel));

    if (colorizeSerialLogging)
        Serial.print(COLOR_RESET);

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


    if (colorizeSerialLogging && loggingLevel >= ERROR)
        Serial.print(BACKGROUND_COLOR_RED);

    Serial.print(text);

    if (colorizeSerialLogging && loggingLevel >= ERROR)
        Serial.print(COLOR_RESET);

    Serial.println();
}

void Log::appendMsgToFile(const uint64_t timestamp, const LoggingLevel loggingLevel, const String& text) const
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

String Log::getLoggingLevelChar(const LoggingLevel level)
{
    switch (level)
    {
    case DEBUG:
        return "D";
    case INFO:
        return "I";
    case WARNING:
        return "W";
    case ERROR:
        return "E";
    case CRITICAL:
        return "C";
    default:
        throw std::invalid_argument("Invalid logging level");
    }
}

String Log::getLoggingLevelColor(const LoggingLevel level)
{
    switch (level)
    {
    case DEBUG:
        return COLOR_GREEN;
    case INFO:
        return COLOR_BLUE;
    case WARNING:
        return COLOR_YELLOW;
    case ERROR:
        return COLOR_RED;
    case CRITICAL:
        return COLOR_MAGENTA;
    default:
        throw std::invalid_argument("Invalid logging level");
    }
}
