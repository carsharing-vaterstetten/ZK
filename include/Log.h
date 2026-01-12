#pragma once

#include <functional>
#include <Stream.h>
#include <vector>
#include <WString.h>

enum LoggingLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct LogSink
{
    std::reference_wrapper<Stream> stream;
    String name;
    bool timestamps = true;
    bool colorize = false;
    LoggingLevel minLevel = DEBUG;
    bool flushOnError = false;
    bool flushOnEveryLine = false;
};

class Log
{
public:
    using SinkID = uint;

    SinkID addOutputStream(Stream& stream, const String& name, bool timestamps, bool colorize, LoggingLevel minLevel,
                           bool flushOnError = false, bool flushOnEveryLine = false);

    void logMsgln(const String& msg, LoggingLevel level) const;

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

    void flush() const;

protected:
    std::vector<LogSink> sinks;

    static void appendMsgToStream(const LogSink& sink, const String& timestampStr, LoggingLevel level, const String& text);

    static String getLoggingLevelChar(LoggingLevel level);
    static String getLoggingLevelColor(LoggingLevel level);
};
