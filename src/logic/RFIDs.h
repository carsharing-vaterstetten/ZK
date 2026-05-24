#pragma once

#include <cstdint>

class ApiClient;

enum class RfidsChecksumResult
{
    FILES_ARE_EQUAL,
    FILES_DIFFER,
    LOCAL_FILE_DOES_NOT_EXIST,
    UNEXPECTED_STATUS_CODE,
};

namespace RFIDs
{
    bool isRegisteredRFID(uint32_t rfid);
    void downloadRfidsIfChanged(const ApiClient& api);
    bool downloadGPSTrackingConsentedRFIDs(const ApiClient& api);
    bool RFIDConsentsToGPSTrackingTest(uint32_t rfid);
    bool load();
}
