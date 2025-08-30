#pragma once

#include <Arduino.h>

#include "Modem.h"

#define LOGGING_LEVEL_DEBUG 0
#define LOGGING_LEVEL_INFO 1
#define LOGGING_LEVEL_WARNING 2
#define LOGGING_LEVEL_ERROR 3
#define LOGGING_LEVEL_CRITICAL 4

class Log
{
public:
    Log(const bool logTime, const bool logLoggingLevel) : logTime(logTime),
                                                          logLoggingLevel(logLoggingLevel)
    {
    }

    void enableSerialLogging();
    void initSerial(unsigned long baud);
    bool enableSDCardLogging(const String& SDCardLogFileName);
    bool enableFlashLogging(const String& flashLogFileName);

    void stopSerialLogging();
    void stopSDCardLogging();
    void stopFlashLogging();

    void logMsgln(const String& msg, uint8_t level) const;
    void vlogMsgf(uint8_t level, const char* msg, va_list args) const;
    void logMsg(uint8_t level, const String& msg) const;
    void logMsgf(uint8_t level, const char* msg, ...) const;
    void write(const uint8_t* buffer, size_t size) const;

    void debugf(const char* msg, ...) const;

    void debugln(const String& msg) const
    {
        logMsgln(msg, LOGGING_LEVEL_DEBUG);
    }

    void infof(const char* msg, ...) const;

    void infoln(const String& msg) const
    {
        logMsgln(msg, LOGGING_LEVEL_INFO);
    }

    void warningf(const char* msg, ...) const;

    void warningln(const String& msg) const
    {
        logMsgln(msg, LOGGING_LEVEL_WARNING);
    }

    void errorf(const char* msg, ...) const;

    void errorln(const String& msg) const
    {
        logMsgln(msg, LOGGING_LEVEL_ERROR);
    }

    void criticalf(const char* msg, ...) const;

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
    String SDCardLogPath;
    String flashLogPath;
    bool isInitialized = false;
    bool logToSDCard = false;
    bool logToFlash = false;
    bool logToSerial = false;
    const bool logTime;
    const bool logLoggingLevel;

    void appendLineToFileOnSDCard(const String& msg) const;
    void appendLineToFileOnFlash(const String& msg) const;
    void writeLineToSerial(const String& msg) const;

    void appendToFileOnSDCard(const String& msg) const;
    void appendToFileOnFlash(const String& msg) const;
    void writeToSerial(const String& msg) const;

    static String getLoggingLevelChar(uint8_t level);
    static String getLoggingLevelColor(uint8_t level);
    static String getTimeString();
};
