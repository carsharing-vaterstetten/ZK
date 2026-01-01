#pragma once

#include <cstdint>
#include <cstddef>

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

#define GPS_DATA_SIZE sizeof(GPS_DATA_t)
#define GPS_LOG_BUFFER_SIZE 20U

class GPS
{
    GPS_DATA_t logBuffer[GPS_LOG_BUFFER_SIZE];
    uint16_t logBufferIndex = 0;
    const char* localFilePath;
    const char* fileUploadEndpoint;

    bool writeLogBufferToFile() const;
    void logDataBuffered(const GPS_DATA_t& data);

public:
    explicit GPS(const char* filePath, const char* endpoint);

    void uploadFileAndDelete(bool deleteIfSuccess, bool deleteAfterRetrying, size_t retries) const;
    static bool getGpsDataAndWriteToFile();
    bool flush();
};
