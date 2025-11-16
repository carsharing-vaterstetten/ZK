#pragma once
#include <cstdint>

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
    void generateChecksum(uint8_t out[16]);
    void downloadRfidsIfChanged();
    bool downloadGPSTrackingConsentedRFIDs();
    bool RFIDConsentsToGPSTrackingTest(uint32_t rfid);
}
