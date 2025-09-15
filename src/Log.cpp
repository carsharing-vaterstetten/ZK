#include "Log.h"

#include <SPIFFS.h>
#include <SD.h>

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

/// Make sure the pins of the sd card aren't used by something else!
bool Log::enableSDCardLogging(const String& SDCardLogFileName, const uint8_t loggingLevel)
{
    if (!SD.exists(SDCardLogFileName))
    {
        // Let's ignore directories for now
        File file = SD.open(SDCardLogFileName, FILE_WRITE, true);
        if (!file)
            return false;

        file.close();
    }

    SDCardLogPath = SDCardLogFileName;
    logToSDCard = true;
    sdCardLoggingLevel = loggingLevel;
    return true;
}

bool Log::enableFlashLogging(const String& flashLogFileName, const uint8_t loggingLevel)
{
    if (!SPIFFS.exists(flashLogFileName))
    {
        // Let's ignore directories for now
        File file = SPIFFS.open(flashLogFileName, FILE_WRITE, true);
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

void Log::stopSDCardLogging()
{
    logToSDCard = false;
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

    char timeStr[HelperUtils::dateTimeStrLength]; // For human-readable time

    if (Modem::isInitialized())
    {
        int year, month, day, hour, minute, second;
        float timezone;
        Modem::getNetworkTime(&year, &month, &day, &hour, &minute, &second, &timezone);
        HelperUtils::dateTimeToString(timeStr, year, month, day, hour, minute, second);
    }
    else
    {
        sniprintf(timeStr, sizeof(timeStr), "%lu", millis());
    }


    if (logToSerial && level >= serialLoggingLevel)
    {
        Serial.printf("[%s][%s%s%s]%s%s\n",
                      timeStr,
#if COLORIZE_SERIAL_LOGGING
                      getLoggingLevelColor(level).c_str(),
#else
                      "",
#endif
                      getLoggingLevelChar(level).c_str(),
#if COLORIZE_SERIAL_LOGGING
                      COLOR_RESET,
#else
                      "",
#endif
                      serialLoggingName.isEmpty() ? " " : ("[" + serialLoggingName + "] ").c_str(),
#if COLORIZE_SERIAL_LOGGING
                      (level >= LOGGING_LEVEL_ERROR)
                          ? (BACKGROUND_COLOR_RED + msg + COLOR_RESET).c_str()
                          : msg.c_str()
#else
                      msg.c_str()
#endif
        );
    }

    if (logToFlash && level >= flashLoggingLevel)
        appendRawMsgToFileOnFlash(timestampMs, level, msg);
    if (logToSDCard && level >= sdCardLoggingLevel)
        appendRawMsgToFileOnSDCard(timestampMs, level, msg);
}


/// Does not add any time and level information
void Log::write(const uint8_t* buffer, const size_t size) const
{
    if (!buffer || size == 0) return;

    if (logToSerial)
    {
        Serial.write(buffer, size);
    }

    if (logToFlash)
    {
        File file = SPIFFS.open(flashLogPath, FILE_APPEND);
        file.write(buffer, size);
        file.close();
    }

    if (logToSDCard)
    {
        File file = SD.open(SDCardLogPath, FILE_APPEND);
        file.write(buffer, size);
        file.close();
    }
}

void Log::appendRawMsgToFile(FS& fs, const String& path, const uint64_t timestamp, const uint8_t loggingLevel,
                             const String& text)
{
    File file = fs.open(path, FILE_APPEND);
    if (!file) return;

    // Write 8-byte timestamp
    file.write(reinterpret_cast<const uint8_t*>(&timestamp), sizeof(timestamp));

    // Write 1-byte logging level
    file.write(&loggingLevel, sizeof(loggingLevel));

    // Write string length + data
    const uint16_t len = text.length();
    file.write(reinterpret_cast<const uint8_t*>(&len), sizeof(len));
    file.write(reinterpret_cast<const uint8_t*>(text.c_str()), len);

    file.close();
}

void Log::appendRawMsgToFileOnSDCard(const uint64_t timestamp, const uint8_t loggingLevel, const String& text) const
{
    appendRawMsgToFile(SD, SDCardLogPath, timestamp, loggingLevel, text);
}

void Log::appendRawMsgToFileOnFlash(const uint64_t timestamp, const uint8_t loggingLevel, const String& text) const
{
    appendRawMsgToFile(SPIFFS, flashLogPath, timestamp, loggingLevel, text);
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
