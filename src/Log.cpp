#include "../include/Log.h"

#include <SPIFFS.h>

#include "SD_MMC.h"

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLORIZE_SERIAL

void Log::enableSerialLogging()
{
    logToSerial = true;
}

/// Make sure the pins of the sd card aren't used by something else!
bool Log::enableSDCardLogging(const String& SDCardLogFileName)
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
    return true;
}

bool Log::enableFlashLogging(const String& flashLogFileName)
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

/// Maximum of 127 chars
void Log::debugf(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    vlogMsgf(LOGGING_LEVEL_DEBUG, msg, args);
    va_end(args);
}

void Log::infof(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    vlogMsgf(LOGGING_LEVEL_INFO, msg, args);
    va_end(args);
}

void Log::warningf(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    vlogMsgf(LOGGING_LEVEL_WARNING, msg, args);
    va_end(args);
}

void Log::errorf(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    vlogMsgf(LOGGING_LEVEL_ERROR, msg, args);
    va_end(args);
}

void Log::criticalf(const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    vlogMsgf(LOGGING_LEVEL_CRITICAL, msg, args);
    va_end(args);
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
    logMsg(level, msg + "\n");
}

void Log::vlogMsgf(const uint8_t level, const char* msg, const va_list args) const
{
    if (!logToSerial) return;

    char buf[128]; // static buffer to avoid heap fragmentation
    const int len = vsnprintf(buf, sizeof(buf), msg, args);

    if (len <= 0) return;

    // If message is longer than buffer, truncate safely
    buf[sizeof(buf) - 1] = '\0';

    logMsg(level, buf);
}

void Log::logMsg(const uint8_t level, const String& msg) const
{
    String finalStr = "";

    const String timeStr = getTimeString();

    if (logTime)
    {
        finalStr += "[" + timeStr + "] ";
    }

    String finalSerialStr = finalStr;

    if (logLoggingLevel)
    {
        finalStr += "[" + getLoggingLevelChar(level) + "] ";
        finalSerialStr += "[";
#ifdef COLORIZE_SERIAL
        finalSerialStr += getLoggingLevelColor(level);
#endif
        finalSerialStr += getLoggingLevelChar(level);
#ifdef COLORIZE_SERIAL
        finalSerialStr += COLOR_RESET;
#endif
        finalSerialStr += "] ";
    }

    finalStr += msg;
    finalSerialStr += msg;

    writeToSerial(finalSerialStr);
    appendToFileOnFlash(finalStr);
    appendToFileOnSDCard(finalStr);
}


void Log::logMsgf(const uint8_t level, const char* msg, ...) const
{
    va_list args;
    va_start(args, msg);
    vlogMsgf(level, msg, args);
    va_end(args);
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
    appendToFileOnSDCard(msg + "\n");
}

void Log::appendLineToFileOnFlash(const String& msg) const
{
    appendToFileOnFlash(msg + "\n");
}

void Log::writeLineToSerial(const String& msg) const
{
    writeToSerial(msg + "\n");
}


void Log::appendToFileOnSDCard(const String& msg) const
{
    if (!logToSDCard) return;

    File file = SD_MMC.open(SDCardLogPath, FILE_APPEND);

    file.print(msg);
    file.close();
}

void Log::appendToFileOnFlash(const String& msg) const
{
    if (!logToFlash) return;

    File file = SPIFFS.open(flashLogPath, FILE_APPEND);
    file.print(msg);
    file.close();
}

void Log::writeToSerial(const String& msg) const
{
    if (!logToSerial) return;

    Serial.print(msg);
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
