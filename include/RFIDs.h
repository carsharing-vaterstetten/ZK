#pragma once
#include <cstdint>

enum class RfidsChecksumResult
{
    FILES_ARE_EQUAL,
    FILES_DIFFER,
    LOCAL_FILE_DOES_NOT_EXIST,
    UNEXPECTED_STATUS_CODE,
};

class RFIDs
{
public:
    static bool isRegisteredRFID(uint32_t rfid);
    static void generateChecksum(uint8_t out[16]);
    static void downloadRfidsIfChanged();
    static bool downloadGPSTrackingConsentedRFIDs();
    static bool RFIDConsentsToGPSTrackingTest(uint32_t rfid);
};
