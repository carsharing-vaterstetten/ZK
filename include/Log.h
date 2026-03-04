#pragma once

#include <functional>
#include <Print.h>
#include <vector>
#include <WString.h>

enum class LoggingLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct LogSink
{
    std::reference_wrapper<Print> print;
    String name;
    bool timestamps = true;
    bool colorize = false;
    LoggingLevel minLevel = LoggingLevel::DEBUG;
    bool flushOnError = false;
    bool flushOnEveryLine = false;
};

class Log
{
public:
    using SinkID = uint;

    SinkID addOutputSink(Print& p, const String& name, bool timestamps, bool colorize, LoggingLevel minLevel,
                           bool flushOnError = false, bool flushOnEveryLine = false);

    void logMsgln(const String& msg, LoggingLevel level) const;

    void debugln(const String& msg) const
    {
        logMsgln(msg, LoggingLevel::DEBUG);
    }

    void infoln(const String& msg) const
    {
        logMsgln(msg, LoggingLevel::INFO);
    }

    void warningln(const String& msg) const
    {
        logMsgln(msg, LoggingLevel::WARNING);
    }

    void errorln(const String& msg) const
    {
        logMsgln(msg, LoggingLevel::ERROR);
    }

    void criticalln(const String& msg) const
    {
        logMsgln(msg, LoggingLevel::CRITICAL);
    }

    void logInfoOrLevelln(bool success, const String& ifSuccess, const String& ifError, LoggingLevel level) const;

    void logInfoOrWarningln(const bool success, const String& ifSuccess, const String& ifError) const
    {
        logInfoOrLevelln(success, ifSuccess, ifError, LoggingLevel::WARNING);
    }

    void logInfoOrErrorln(const bool success, const String& ifSuccess, const String& ifError) const
    {
        logInfoOrLevelln(success, ifSuccess, ifError, LoggingLevel::ERROR);
    }

    void logInfoOrCriticalErrorln(const bool success, const String& ifSuccess, const String& ifError) const
    {
        logInfoOrLevelln(success, ifSuccess, ifError, LoggingLevel::CRITICAL);
    }

    void flush() const;

protected:
    std::vector<LogSink> sinks;

    static void appendMsgToSink(const LogSink& sink, const String& timestampStr, LoggingLevel level, const String& text);

    static String getLoggingLevelChar(LoggingLevel level);
    static String getLoggingLevelColor(LoggingLevel level);
};
