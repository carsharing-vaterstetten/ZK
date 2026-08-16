#include "domain/GPS.h"

#include <LittleFS.h>

#include "logging/Loggers.h"
#include "hal/Modem.h"

GPS::GPS(const char* localFilePath, const char* uploadEndpoint) : localFilePath(localFilePath),
                                                                  uploadEndpoint(uploadEndpoint) {}

bool GPS::begin()
{
    if (gpsFile) return true;
    gpsFile = LittleFS.open(localFilePath, FILE_APPEND, true);
    return gpsFile;
}

void GPS::uploadFileAndBeginNew(ApiClient& api, const bool deleteIfSuccess, const bool deleteAfterRetrying,
                                const uint retries)
{
    fileLog.infoln("Uploading GPS log (" + String(fileSize()) + " B)");

    if (gpsFile) gpsFile.close();

    if (!LittleFS.exists(localFilePath)) return;

    Modem::uploadFileAndDelete(api, uploadEndpoint, localFilePath, deleteIfSuccess, deleteAfterRetrying, retries);

    gpsFile = LittleFS.open(localFilePath, FILE_APPEND, true);
}

bool GPS::writeData(const GPS_DATA_t& data)
{
    if (!gpsFile) return false;
    const auto dataBytes = reinterpret_cast<const uint8_t*>(&data);
    const size_t dataWritten = gpsFile.write(dataBytes, sizeof(GPS_DATA_t));
    return dataWritten == sizeof(GPS_DATA_t);
}

size_t GPS::fileSize() const
{
    return gpsFile ? gpsFile.size() : 0;
}
