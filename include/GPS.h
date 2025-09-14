#pragma once

#include <cstdint>

#pragma pack(push, 1)
struct GPS_DATA_t
{
    float lat = 0.0;
    float lon = 0.0;
    float speed = 0;
    float alt = 0;
    uint8_t vsat = 0;
    uint8_t usat = 0;
    float accuracy = 0;
    uint16_t year = 0;
    uint8_t month = 0;
    uint8_t day = 0;
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
};
#pragma pack(pop)

class GPS
{
public:
    static void uploadFileFromAllFilesystems(bool deleteIfSuccess, bool deleteAfterRetrying, uint32_t retries);
    static void logData(const GPS_DATA_t& data);
};
