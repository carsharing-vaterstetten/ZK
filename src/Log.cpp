#include "Log.h"

#include "HelperUtils.h"
#include "Modem.h"

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define BACKGROUND_COLOR_RED "\033[41m"

Log::SinkID Log::addOutputSink(Print& p, const String& name, const bool timestamps, const bool colorize,
                               const LoggingLevel minLevel, const bool flushOnError, const bool flushOnEveryLine)
{
    sinks.push_back({p, name, timestamps, colorize, minLevel, flushOnError, flushOnEveryLine});
    return sinks.size() - 1;
}

void Log::logInfoOrLevelln(const bool success, const String& ifSuccess, const String& ifError,
                           const LoggingLevel level) const
{
    if (success)
        infoln(ifSuccess);
    else
        logMsgln(ifError, level);
}

void Log::logMsgln(const String& msg, const LoggingLevel level) const
{
    // = millis() if modem is not initialized
    const uint64_t timestampMs = HelperUtils::systemTimeMillisecondsSinceEpoche();

    for (const LogSink& s : sinks)
    {
        if (level >= s.minLevel)
        {
            const String timestampStr = s.timestamps ? "[" + HelperUtils::millisToIsoString(timestampMs) + "]" : "";
            appendMsgToSink(s, timestampStr, level, msg);
        }
    }
}

void Log::flush() const
{
    for (const LogSink& s : sinks)
        s.print.get().flush();
}

void Log::appendMsgToSink(const LogSink& sink, const String& timestampStr, const LoggingLevel level,
                          const String& text)
{
    Print& p = sink.print.get();

    String line;

    if (sink.timestamps) line += timestampStr;

    line += "[";
    if (sink.colorize) line += getLoggingLevelColor(level);
    line += getLoggingLevelChar(level);
    if (sink.colorize) line += COLOR_RESET;
    line += "]";

    if (sink.name && sink.name[0] != '\0') line += "[" + String(sink.name) + "] ";
    else line += " ";

    if (sink.colorize && level >= LoggingLevel::ERROR) line += BACKGROUND_COLOR_RED;
    line += text;
    if (sink.colorize && level >= LoggingLevel::ERROR) line += COLOR_RESET;

    const size_t written = p.print(line + "\n");

    if (written - 1 != line.length())
    {
        Serial.println("Failed to write '" + line + "' to sink '" + sink.name + "'");
    }

    if (sink.flushOnEveryLine || (sink.flushOnError && level >= LoggingLevel::ERROR)) p.flush();
}

String Log::getLoggingLevelChar(const LoggingLevel level)
{
    switch (level)
    {
    case LoggingLevel::DEBUG:
        return "D";
    case LoggingLevel::INFO:
        return "I";
    case LoggingLevel::WARNING:
        return "W";
    case LoggingLevel::ERROR:
        return "E";
    case LoggingLevel::CRITICAL:
        return "C";
    default:
        throw std::invalid_argument("Invalid logging level");
    }
}

String Log::getLoggingLevelColor(const LoggingLevel level)
{
    switch (level)
    {
    case LoggingLevel::DEBUG:
        return COLOR_GREEN;
    case LoggingLevel::INFO:
        return COLOR_BLUE;
    case LoggingLevel::WARNING:
        return COLOR_YELLOW;
    case LoggingLevel::ERROR:
        return COLOR_RED;
    case LoggingLevel::CRITICAL:
        return COLOR_MAGENTA;
    default:
        throw std::invalid_argument("Invalid logging level");
    }
}
