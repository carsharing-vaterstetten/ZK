#pragma once
#include <WString.h>

enum LoggingLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Log
{
public:
    void enableSerialLogging(bool colorize, LoggingLevel loggingLevel = DEBUG, const String& serialName = "");
    bool enableFlashLogging(const String& flashLogFileName, LoggingLevel loggingLevel = INFO);

    void stopSerialLogging();
    void stopFlashLogging();

    void logMsgln(const String& msg, LoggingLevel level) const;
    void appendMsgToSerial(uint64_t timestamp, LoggingLevel loggingLevel, const String& text) const;

    void debugln(const String& msg) const
    {
        logMsgln(msg, DEBUG);
    }

    void infoln(const String& msg) const
    {
        logMsgln(msg, INFO);
    }

    void warningln(const String& msg) const
    {
        logMsgln(msg, WARNING);
    }

    void errorln(const String& msg) const
    {
        logMsgln(msg, ERROR);
    }

    void criticalln(const String& msg) const
    {
        logMsgln(msg, CRITICAL);
    }

    void logInfoOrLevelln(bool success, const String& ifSuccess, const String& ifError, LoggingLevel level) const;

    void logInfoOrWarningln(const bool success, const String& ifSuccess, const String& ifError) const
    {
        logInfoOrLevelln(success, ifSuccess, ifError, WARNING);
    }

    void logInfoOrErrorln(const bool success, const String& ifSuccess, const String& ifError) const
    {
        logInfoOrLevelln(success, ifSuccess, ifError, ERROR);
    }

    void logInfoOrCriticalErrorln(const bool success, const String& ifSuccess, const String& ifError) const
    {
        logInfoOrLevelln(success, ifSuccess, ifError, CRITICAL);
    }

private:
    String flashLogPath;
    String serialLoggingName;
    bool logToFlash = false;
    bool logToSerial = false;
    bool colorizeSerialLogging = true;

    LoggingLevel serialLoggingLevel = DEBUG;
    LoggingLevel flashLoggingLevel = DEBUG;

    void appendMsgToFile(uint64_t timestamp, LoggingLevel loggingLevel, const String& text) const;

    static String getLoggingLevelChar(LoggingLevel level);
    static String getLoggingLevelColor(LoggingLevel level);
};
