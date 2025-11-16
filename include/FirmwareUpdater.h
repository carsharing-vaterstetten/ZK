#pragma once

enum class FirmwareUpdateCheckResult
{
    NO_UPDATE_AVAILABLE,
    UPDATE_AVAILABLE,
    ERROR,
    UNKNOWN_RESPONSE,
};

namespace FirmwareUpdater
{
    namespace detail
    {
        bool performUpdate();
    }

    FirmwareUpdateCheckResult checkForFirmwareUpdate();
    bool doUpdateIfAvailable();
}
