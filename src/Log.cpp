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

Log::SinkID Log::addOutputStream(Stream& stream, const String& name, const bool timestamps, const bool colorize,
                                 const LoggingLevel minLevel, const bool flushOnError, const bool flushOnEveryLine)
{
    sinks.push_back({stream, name, timestamps, colorize, minLevel, flushOnError, flushOnEveryLine});
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
            appendMsgToStream(s, timestampStr, level, msg);
        }
    }
}

void Log::flush() const
{
    for (const LogSink& s : sinks)
        s.stream.get().flush();
}

uint64_t mst = 0;

void Log::appendMsgToStream(const LogSink& sink, const String& timestampStr, const LoggingLevel level,
                            const String& text)
{
    Stream& stream = sink.stream.get();
    String line;

    if (sink.timestamps) line += timestampStr;

    line += "[";
    if (sink.colorize) line += getLoggingLevelColor(level);
    line += getLoggingLevelChar(level);
    if (sink.colorize) line += COLOR_RESET;
    line += "]";

    if (sink.name && sink.name[0] != '\0') line += "[" + String(sink.name) + "] ";
    else line += " ";

    if (sink.colorize && level >= ERROR) line += BACKGROUND_COLOR_RED;
    line += text;
    if (sink.colorize && level >= ERROR) line += COLOR_RESET;

    stream.println(line);

    if (sink.flushOnEveryLine || (sink.flushOnError && level >= ERROR)) stream.flush();
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
