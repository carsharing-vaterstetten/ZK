#pragma once

#include <cstdint>
#include <FS.h>
#include <sys/types.h>

#pragma pack(push, 1)
struct GPS_DATA_t
{
    float lat = 0.0f;
    float lon = 0.0f;
    float speed = 0.0f;
    float alt = 0.0f;
    uint8_t vsat = 0;
    uint8_t usat = 0;
    float accuracy = 0.0f;
    uint64_t unixTimestamp = 0;
};
#pragma pack(pop)

class GPS
{
public:
    GPS(const char* localFilePath, const char* uploadEndpoint);

    bool writeData(const GPS_DATA_t& data);
    bool uploadFileAndBeginNew(bool deleteIfSuccess, bool deleteAfterRetrying, uint retries);
    bool begin();
    bool flush();
    void end();
    [[nodiscard]] size_t fileSize() const;

protected:
    File gpsFile;
    const char* localFilePath;
    const char* uploadEndpoint;
};
