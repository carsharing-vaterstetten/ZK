#include "GPS.h"

#include <LittleFS.h>

#include "Globals.h"

GPS::GPS(const char* localFilePath, const char* uploadEndpoint) : localFilePath(localFilePath),
                                                                  uploadEndpoint(uploadEndpoint) {}

bool GPS::begin()
{
    if (gpsFile) return true;
    gpsFile = LittleFS.open(localFilePath, FILE_APPEND, true);
    return gpsFile;
}

bool GPS::uploadFileAndBeginNew(const bool deleteIfSuccess, const bool deleteAfterRetrying, const uint retries)
{
    fileLog.infoln("Uploading GPS log (" + String(fileSize()) + " B)");

    if (gpsFile) gpsFile.close();

    const UploadFileAndRetryResult uploadResult = Modem::uploadFileAndDelete(
        uploadEndpoint, localFilePath, deleteIfSuccess, deleteAfterRetrying, retries);

    gpsFile = LittleFS.open(localFilePath, FILE_APPEND, true);

    return gpsFile && (uploadResult == UploadFileAndRetryResult::SUCCESS || uploadResult ==
        UploadFileAndRetryResult::SUCCESS_AFTER_RETRYING);
}

bool GPS::writeData(const GPS_DATA_t& data)
{
    if (!gpsFile) return false;
    const auto dataBytes = reinterpret_cast<const uint8_t*>(&data);
    const size_t dataWritten = gpsFile.write(dataBytes, sizeof(GPS_DATA_t));
    return dataWritten == sizeof(GPS_DATA_t);
}

bool GPS::flush()
{
    if (!gpsFile) return false;
    gpsFile.flush();
    return true;
}

void GPS::end()
{
    if (!gpsFile) return;
    gpsFile.close();
}

size_t GPS::fileSize() const
{
    return gpsFile ? gpsFile.size() : 0;
}
