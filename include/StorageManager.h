#pragma once

#include <LittleFS.h>

#include "Intern.h"

class StorageManager
{
    static bool flashIsMounted;

public:
    StorageManager() = delete; // Prevent instantiation

    static bool mountLittleFS();

    static File openLog(const char* mode, const bool create = false)
    {
        return LittleFS.open(LOG_FILE_PATH, mode, create);
    }

    static bool removeLog()
    {
        return LittleFS.remove(LOG_FILE_PATH);
    }

    static File openRFIDs(const char* mode, const bool create = false)
    {
        return LittleFS.open(RFID_FILE_PATH, mode, create);
    }

    static File openTmpRFIDs(const char* mode, const bool create = false)
    {
        return LittleFS.open(TMP_RFID_FILE_PATH, mode, create);
    }

    static File openFirmware(const char* mode, const bool create = false)
    {
        return LittleFS.open(FIRMWARE_FILE_PATH, mode, create);
    }

    static bool replaceRFIDsFileWithTmpRFIDs();

    static bool remove(const String& path, bool notExistingOk = true);

    static bool removeTmpRFIDs(const bool notExistingOk = true)
    {
        return remove(TMP_RFID_FILE_PATH, notExistingOk);
    }

    static bool removeRFIDs(const bool notExistingOk = true)
    {
        return remove(RFID_FILE_PATH, notExistingOk);
    }

    static bool exists(const String& path)
    {
        return LittleFS.exists(path);
    }

    static bool rfidsFileExists()
    {
        return exists(RFID_FILE_PATH);
    }

    static bool removeFirmwareFile(const bool notExistingOk = true)
    {
        return remove(FIRMWARE_FILE_PATH, notExistingOk);
    }

    static void logDirTree(const char* dirname, const uint8_t maxDepth)
    {
        logDirTree(dirname, maxDepth, 0);
    }
    static void logFilesystemTree(uint8_t maxDepth);

    static void logFilesystemsInformation();

private:
    static void logDirTree(const char* dirname, uint8_t maxDepth, uint8_t indent);
};
