#pragma once

#include <cstdint>
#include <ctime>
#include <WString.h>

namespace Time
{
    time_t toUnixTimestamp(int year, int month, int day, int hour, int minute, int second, float timezone);

    /// Falls back to millis() while the system clock is unset.
    uint64_t millisSinceEpoch();

    String toIsoString(uint64_t ms);

    void syncSystemTime(time_t unixTimestamp);
}
