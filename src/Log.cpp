#include "Log.h"

#include <SPIFFS.h>

#include "Config.h"
#include "Modem.h"
#include "SD_MMC.h"

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define BACKGROUND_COLOR_RED "\033[41m"

void Log::enableSerialLogging(const uint8_t loggingLevel)
{
    logToSerial = true;
    serialLoggingLevel = loggingLevel;
}

/// Make sure the pins of the sd card aren't used by something else!
bool Log::enableSDCardLogging(const String& SDCardLogFileName, const uint8_t loggingLevel)
{
    if (!SD_MMC.exists(SDCardLogFileName))
    {
        // Let's ignore directories for now
        File file = SD_MMC.open(SDCardLogFileName, FILE_WRITE, true);
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
    String finalStr = "";

    const String timeStr = getTimeString();

    if (logTime)
    {
        finalStr += "[" + timeStr + "]";
    }

    String finalSerialStr = finalStr;

    if (logLoggingLevel)
    {
        finalStr += "[" + getLoggingLevelChar(level) + "]";
        finalSerialStr += "[";
#if COLORIZE_SERIAL_LOGGING
        finalSerialStr += getLoggingLevelColor(level);
#endif
        finalSerialStr += getLoggingLevelChar(level);
#if COLORIZE_SERIAL_LOGGING
        finalSerialStr += COLOR_RESET;
#endif
        finalSerialStr += "]";
    }

    String nameString = "";

    if (!loggerName.isEmpty())
    {
        nameString = "[" + loggerName + "]";
    }

    finalStr += nameString;
    finalSerialStr += nameString;

    finalStr += " " + msg;

#if COLORIZE_SERIAL_LOGGING
    if (level >= LOGGING_LEVEL_ERROR)
        finalSerialStr += " " BACKGROUND_COLOR_RED + msg + COLOR_RESET;
    else
#endif
        finalSerialStr += " " + msg;

    if (level >= serialLoggingLevel)
        writeLineToSerial(finalSerialStr);
    if (level >= flashLoggingLevel)
        appendLineToFileOnFlash(finalStr);
    if (level >= sdCardLoggingLevel)
        appendLineToFileOnSDCard(finalStr);
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
        File file = SD_MMC.open(SDCardLogPath, FILE_APPEND);
        file.write(buffer, size);
        file.close();
    }
}

void Log::appendLineToFileOnSDCard(const String& msg) const
{
    if (!logToSDCard) return;

    File file = SD_MMC.open(SDCardLogPath, FILE_APPEND);
    file.println(msg);
    file.close();
}

void Log::appendLineToFileOnFlash(const String& msg) const
{
    if (!logToFlash) return;

    File file = SPIFFS.open(flashLogPath, FILE_APPEND);
    file.println(msg);
    file.close();
}

void Log::writeLineToSerial(const String& msg) const
{
    if (!logToSerial) return;
    Serial.println(msg);
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

String Log::getTimeString()
{
    if (!Modem::isInitialized())
    {
        // Fallback solution
        return String(millis());
    }

    const String currentTime = Modem::getLocalTime();
    // Zeitformat "24/11/03,15:01:03+04" (YY/MM/DD,HH:MM:SS+TZ)
    return currentTime;
}
