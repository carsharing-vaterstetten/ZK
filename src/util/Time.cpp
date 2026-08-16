#include "util/Time.h"

#include <cstdio>
#include <sys/time.h>

time_t Time::toUnixTimestamp(const int year, const int month, const int day, const int hour,
                             const int minute, const int second, const float timezone)
{
    tm datetime{};

    datetime.tm_year = year - 1900;
    datetime.tm_mon = month - 1;
    datetime.tm_mday = day;
    datetime.tm_hour = hour;
    datetime.tm_min = minute;
    datetime.tm_sec = second;
    datetime.tm_isdst = -1;

    return mktime(&datetime) - static_cast<time_t>(timezone * 3600.0f);
}

uint64_t Time::millisSinceEpoch()
{
    // Deliberately not static: called concurrently from every task via the
    // loggers, and a shared scratch buffer produced torn timestamps.
    timeval now{};
    gettimeofday(&now, nullptr);
    return now.tv_sec * 1000ULL + now.tv_usec / 1000ULL;
}

String Time::toIsoString(const uint64_t ms)
{
    const auto seconds = static_cast<time_t>(ms / 1000ULL);
    tm timeinfo{};
    gmtime_r(&seconds, &timeinfo);

    char buf[30];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &timeinfo);

    char result[40];
    snprintf(result, sizeof(result), "%s.%03uZ", buf, static_cast<unsigned>(ms % 1000));
    return {result};
}

void Time::syncSystemTime(const time_t unixTimestamp)
{
    const timeval now = {.tv_sec = unixTimestamp, .tv_usec = 0};
    settimeofday(&now, nullptr);
}
