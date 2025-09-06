#pragma once
#include <cstdint>

enum class RfidsChecksumResult
{
    FILES_ARE_EQUAL,
    FILES_DIFFER,
    LOCAL_FILE_DOES_NOT_EXIST,
    ERROR,
};

class RFIDs
{
public:
    static bool isRegisteredRFID(uint32_t rfid);
    static RfidsChecksumResult compareChecksums();
    static bool downloadRfids();
    static void downloadRfidsIfChanged();
    static bool downloadGPSTrackingConsentedRFIDs();
    static bool RFIDConsentsToGPSTrackingTest(uint32_t rfid);
};
