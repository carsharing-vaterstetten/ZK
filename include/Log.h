#pragma once

#include <Arduino.h>
#include <FS.h>

#define LOGGING_LEVEL_DEBUG 0
#define LOGGING_LEVEL_INFO 1
#define LOGGING_LEVEL_WARNING 2
#define LOGGING_LEVEL_ERROR 3
#define LOGGING_LEVEL_CRITICAL 4

class Log
{
public:
    void enableSerialLogging(uint8_t loggingLevel = LOGGING_LEVEL_DEBUG, const String& serialName = "");
    bool enableFlashLogging(const String& flashLogFileName, uint8_t loggingLevel = LOGGING_LEVEL_INFO);

    void stopSerialLogging();
    void stopFlashLogging();

    void logMsgln(const String& msg, uint8_t level) const;
    void write(const uint8_t* buffer, size_t size) const;

    void debugln(const String& msg) const
    {
        logMsgln(msg, LOGGING_LEVEL_DEBUG);
    }

    void infoln(const String& msg) const
    {
        logMsgln(msg, LOGGING_LEVEL_INFO);
    }

    void warningln(const String& msg) const
    {
        logMsgln(msg, LOGGING_LEVEL_WARNING);
    }

    void errorln(const String& msg) const
    {
        logMsgln(msg, LOGGING_LEVEL_ERROR);
    }

    void criticalln(const String& msg) const
    {
        logMsgln(msg, LOGGING_LEVEL_CRITICAL);
    }

    void logInfoOrLevelln(bool success, const String& ifSuccess, const String& ifError, uint8_t level) const;

    void logInfoOrWarningln(const bool success, const String& ifSuccess, const String& ifError) const
    {
        logInfoOrLevelln(success, ifSuccess, ifError, LOGGING_LEVEL_WARNING);
    }

    void logInfoOrErrorln(const bool success, const String& ifSuccess, const String& ifError) const
    {
        logInfoOrLevelln(success, ifSuccess, ifError, LOGGING_LEVEL_ERROR);
    }

    void logInfoOrCriticalErrorln(const bool success, const String& ifSuccess, const String& ifError) const
    {
        logInfoOrLevelln(success, ifSuccess, ifError, LOGGING_LEVEL_CRITICAL);
    }

private:
    String flashLogPath;
    String serialLoggingName;
    bool logToFlash = false;
    bool logToSerial = false;

    uint8_t serialLoggingLevel = LOGGING_LEVEL_DEBUG;
    uint8_t flashLoggingLevel = LOGGING_LEVEL_DEBUG;

    void appendRawMsgToFileOnFlash(uint64_t timestamp, uint8_t loggingLevel, const String& text) const;
    static void appendRawMsgToFile(FS& fs, const String& path, uint64_t timestamp, uint8_t loggingLevel,
                                   const String& text);

    static String getLoggingLevelChar(uint8_t level);
    static String getLoggingLevelColor(uint8_t level);
};
