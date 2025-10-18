#include "GPS.h"

#include "Backend.h"
#include "Globals.h"
#include "Intern.h"
#include "Modem.h"
#include "StorageManager.h"

uint16_t GPS::logBufferIndex = 0;
GPS_DATA_t GPS::logBuffer[GPS_LOG_BUFFER_SIZE];

void GPS::uploadFileAndDelete(const bool deleteIfSuccess, const bool deleteAfterRetrying, const uint32_t retries)
{
    fileLog.infoln("Uploading GPS log(s)");
    Modem::uploadFileAndDelete(GPS_FILE_UPLOAD_ENDPOINT, GPS_FILE_PATH, deleteIfSuccess, deleteAfterRetrying, retries);
}

bool GPS::writeLogBufferToFile()
{
    const auto dataBytes = reinterpret_cast<const uint8_t*>(&logBuffer);
    File f = StorageManager::openGPS(FILE_APPEND, true);
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
