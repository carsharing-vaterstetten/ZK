#pragma once


enum class FirmwareUpdateCheckResult
{
    NO_UPDATE_AVAILABLE,
    UPDATE_AVAILABLE,
    ERROR,
    UNKNOWN_RESPONSE,
};

class FirmwareUpdater
{
    static bool downloadAndPerformUpdate();
    static long getLatestFirmwareSize();
    static bool doFirmwareUpdateWithWatchdog();
public:
    static FirmwareUpdateCheckResult checkForFirmwareUpdate();
    static bool doUpdateIfAvailable();
};
