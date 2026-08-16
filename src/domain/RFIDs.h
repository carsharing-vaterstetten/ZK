#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include "config/Intern.h"

class ApiClient;

/// The authorised UID lists, held sorted in RAM for binary search and backed by
/// files on flash. Lookups run on the card-reader path while downloads replace
/// the lists from the modem task, so each list is a shared_ptr swapped under its
/// own mutex: a reader holding the pointer keeps its snapshot alive.
class RFIDs
{
public:
    RFIDs(const char* filePath, const char* tmpFilePath, const char* gpsFilePath, const char* tmpGpsFilePath);

    bool isRegisteredRFID(uint32_t rfid) const;
    bool RFIDConsentsToGPSTrackingTest(uint32_t rfid) const;

    void downloadRfidsIfChanged(ApiClient& api);
    bool downloadGPSTrackingConsentedRFIDs(ApiClient& api);

    bool loadFromFileToRam();
    bool loadFromGpsFileToRam();

protected:
    using UidList = std::shared_ptr<const std::vector<uint32_t>>;

    UidList getUids() const;
    UidList getGPSUids() const;
    void generateChecksum(uint8_t* out) const;

    static bool loadUidsToRam(const char* path, UidList& target, std::mutex& targetMutex, const char* label);

    UidList rfids;
    UidList gpsRfids;
    mutable std::mutex ramMutex;
    mutable std::mutex gpsRamMutex;

    const char* filePath;
    const char* tmpFilePath;
    const char* gpsFilePath;
    const char* tmpGpsFilePath;
};
