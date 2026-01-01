#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <cstdint>
#include <LittleFS.h>

#include "Intern.h"

namespace StorageManager
{
    static bool flashIsMounted = false;

    bool mountLittleFS();

    inline File openLog(const char* mode, const bool create = false)
    {
        return LittleFS.open(LOG_FILE_PATH, mode, create);
    }

    inline bool removeLog()
    {
        return LittleFS.remove(LOG_FILE_PATH);
    }

    inline File openRFIDs(const char* mode, const bool create = false)
    {
        return LittleFS.open(RFID_FILE_PATH, mode, create);
    }

    inline File openTmpRFIDs(const char* mode, const bool create = false)
    {
        return LittleFS.open(TMP_RFID_FILE_PATH, mode, create);
    }

    inline File openGpsRFIDs(const char* mode, const bool create = false)
    {
        return LittleFS.open(GPS_TRACKING_CONSENTED_RFIDS_FILE_PATH, mode, create);
    }

    inline File openGPS(const char* mode, const bool create = false)
    {
        return LittleFS.open(GPS_FILE_PATH, mode, create);
    }

    bool replaceRFIDsFileWithTmpRFIDs();
    bool replaceGpsUIDsFileWithTmpUIDs();
    bool move(const String& oldPath, const String& newPath, bool deleteIfNewExists);

    bool remove(const String& path, bool notExistingOk = true);

    inline bool removeTmpRFIDs(const bool notExistingOk = true)
    {
        return remove(TMP_RFID_FILE_PATH, notExistingOk);
    }

    inline bool removeRFIDs(const bool notExistingOk = true)
    {
        return remove(RFID_FILE_PATH, notExistingOk);
    }

    inline bool removeGpsLog(const bool notExistingOk = true)
    {
        return remove(GPS_FILE_PATH, notExistingOk);
    }

    inline bool exists(const String& path)
    {
        return LittleFS.exists(path);
    }

    inline bool rfidsFileExists()
    {
        return exists(RFID_FILE_PATH);
    }

    void logDirTree(const char* dirname, size_t maxDepth);

    void logFilesystemTree(size_t maxDepth);

    void logFilesystemsInformation();
}
