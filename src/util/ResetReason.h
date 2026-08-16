#pragma once

#include <rom/rtc.h>
#include <WString.h>

namespace ResetReason
{
    String describe(RESET_REASON reason);
}
