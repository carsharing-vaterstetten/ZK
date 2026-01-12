#include "GPS.h"

#include <LittleFS.h>

#include "Globals.h"

GPS::GPS(const char* filePath, const char* endpoint) : localFilePath(filePath), fileUploadEndpoint(endpoint)
{
}

bool GPS::getGpsDataAndWriteToFile()
{
    GPS_DATA_t data;
    const bool success = modem.getGPS(data);

    if (success)
        gps.logDataBuffered(data);

    return success;
}

void GPS::uploadFileAndDelete(const bool deleteIfSuccess, const bool deleteAfterRetrying, const uint retries) const
{
    fileLog.infoln("Uploading GPS log " + String(fileUploadEndpoint));
    Modem::uploadFileAndDelete(fileUploadEndpoint, localFilePath, deleteIfSuccess, deleteAfterRetrying, retries);
}

bool GPS::writeLogBufferToFile() const
{
    const auto dataBytes = reinterpret_cast<const uint8_t*>(&logBuffer);
    File f = LittleFS.open(localFilePath, FILE_APPEND, true);
    if (!f)
    {
        fileLog.errorln("Failed to open GPS log file for appending");
        return false;
    }
    f.write(dataBytes, GPS_DATA_SIZE * logBufferIndex);
    f.close();
    return true;
}

void GPS::logDataBuffered(const GPS_DATA_t& data)
{
    logBuffer[logBufferIndex] = data;
    ++logBufferIndex;

    if (logBufferIndex >= GPS_LOG_BUFFER_SIZE)
    {
        writeLogBufferToFile();
        logBufferIndex = 0;
    }
}

bool GPS::flush()
{
    const bool success = writeLogBufferToFile();
    if (success)
    {
        logBufferIndex = 0;
    }
    return success;
}
