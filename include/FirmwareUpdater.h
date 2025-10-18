#pragma once
#include <sys/_stdint.h>


enum class FirmwareUpdateCheckResult
{
    NO_UPDATE_AVAILABLE,
    UPDATE_AVAILABLE,
    ERROR,
    UNKNOWN_RESPONSE,
};

class FirmwareUpdater
{
    static bool performUpdate();
    static uint32_t getLatestFirmwareSize();
public:
    static FirmwareUpdateCheckResult checkForFirmwareUpdate();
    static bool doUpdateIfAvailable();
};
