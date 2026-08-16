#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include "config/Intern.h"

class ApiClient;

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
    RFIDs(const char* filePath, const char* tmpFilePath, const char* gpsFilePath, const char* tmpGpsFilePath);

    bool isRegisteredRFID(uint32_t rfid) const;
    void downloadRfidsIfChanged(ApiClient& api);
    bool downloadGPSTrackingConsentedRFIDs(ApiClient& api);
    bool RFIDConsentsToGPSTrackingTest(uint32_t rfid) const;
    bool loadFromFileToRam();
    bool loadFromGpsFileToRam();

protected:
    std::shared_ptr<const std::vector<uint32_t>> getUids() const;
    std::shared_ptr<const std::vector<uint32_t>> getGPSUids() const;
    void generateChecksum(uint8_t* out) const;

    std::shared_ptr<const std::vector<uint32_t>> rfids;
    std::shared_ptr<const std::vector<uint32_t>> gpsRfids;
    mutable std::mutex ramMutex;
    mutable std::mutex gpsRamMutex;

    const char* filePath;
    const char* tmpFilePath;
    const char* gpsFilePath;
    const char* tmpGpsFilePath;
};
