#include "GPS.h"

#include <LittleFS.h>

#include "Backend.h"
#include "Log.h"
#include "Modem.h"

void GPS::uploadFileAndDelete(const bool deleteIfSuccess, const bool deleteAfterRetrying, const uint32_t retries) const
{
    fileLog.infoln("Uploading GPS log(s)");
    modem.uploadFileAndDelete(GPS_FILE_UPLOAD_ENDPOINT, localFilePath, deleteIfSuccess, deleteAfterRetrying, retries);
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
