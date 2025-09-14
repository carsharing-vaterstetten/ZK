#include "GPS.h"

#include "Backend.h"
#include "Globals.h"
#include "Intern.h"
#include "Modem.h"
#include "StorageManager.h"

void GPS::uploadFileFromAllFilesystems(const bool deleteIfSuccess, const bool deleteAfterRetrying,
                                       const uint32_t retries)
{
    fileLog.infoln("Uploading GPS log(s)");
    Modem::uploadFileFromAllFileSystem(GPS_FILE_PATH, GPS_FILE_UPLOAD_ENDPOINT, deleteIfSuccess, deleteAfterRetrying,
                                       retries);
}

void GPS::logData(const GPS_DATA_t& data)
{
    const auto dataBytes = reinterpret_cast<const uint8_t*>(&data);
    File f = StorageManager::openGPS(FILE_APPEND, true);
    f.write(dataBytes, sizeof(GPS_DATA_t));
    f.close();
}
