#include "domain/GpsLog.h"

#include <LittleFS.h>

#include "logging/Loggers.h"
#include "hal/Modem.h"

GpsLog::GpsLog(const char* localFilePath, const char* uploadEndpoint) : localFilePath(localFilePath),
                                                                  uploadEndpoint(uploadEndpoint) {}

bool GpsLog::begin()
{
    if (gpsFile) return true;
    gpsFile = LittleFS.open(localFilePath, FILE_APPEND, true);
    return gpsFile;
}

void GpsLog::uploadFileAndBeginNew(ApiClient& api, const bool deleteIfSuccess, const bool deleteAfterRetrying,
                                const uint retries)
{
    logger.infoln("Uploading GPS log (" + String(fileSize()) + " B)");

    if (gpsFile) gpsFile.close();

    if (!LittleFS.exists(localFilePath)) return;

    Modem::uploadFileAndDelete(api, uploadEndpoint, localFilePath, deleteIfSuccess, deleteAfterRetrying, retries);

    gpsFile = LittleFS.open(localFilePath, FILE_APPEND, true);
}

bool GpsLog::writeData(const GPS_DATA_t& data)
{
    if (!gpsFile) return false;
    const auto dataBytes = reinterpret_cast<const uint8_t*>(&data);
    const size_t dataWritten = gpsFile.write(dataBytes, sizeof(GPS_DATA_t));
    return dataWritten == sizeof(GPS_DATA_t);
}

size_t GpsLog::fileSize() const
{
    return gpsFile ? gpsFile.size() : 0;
}
