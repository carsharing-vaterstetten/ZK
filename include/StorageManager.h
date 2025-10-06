#pragma once

#include <FS.h>

#include "Intern.h"

class StorageManager
{
    static bool sspiffsIsMounted;

public:
    static FS* logFileFs;
    static FS* rfidsFs;
    static FS* firmwareFs;

    StorageManager() = delete; // Prevent instantiation

    static void setFS(FS& logFile, FS& rfids, FS& firmware);
    static bool mountSSPIFFS();

    static File openLog(const char* mode, const bool create = false)
    {
        return logFileFs->open(LOG_FILE_PATH, mode, create);
    }

    static bool removeLog()
    {
        return logFileFs->remove(LOG_FILE_PATH);
    }

    static File openRFIDs(const char* mode, const bool create = false)
    {
        return rfidsFs->open(RFID_FILE_PATH, mode, create);
    }

    static File openTmpRFIDs(const char* mode, const bool create = false)
    {
        return rfidsFs->open(TMP_RFID_FILE_PATH, mode, create);
    }

    static File openFirmware(const char* mode, const bool create = false)
    {
        return firmwareFs->open(FIRMWARE_FILE_PATH, mode, create);
    }

    static bool remove(FS& fs, const String& path, bool notExistingOk = true);

    static bool removeTmpRFIDs(const bool notExistingOk = true)
    {
        return remove(*rfidsFs, TMP_RFID_FILE_PATH, notExistingOk);
    }

    static bool removeRFIDs(const bool notExistingOk = true)
    {
        return remove(*rfidsFs, RFID_FILE_PATH, notExistingOk);
    }

    static bool exists(FS& fs, const String& path)
    {
        return fs.exists(path);
    }

    static bool rfidsFileExists()
    {
        return exists(*rfidsFs, RFID_FILE_PATH);
    }

    static bool removeFirmwareFile(const bool notExistingOk = true)
    {
        return remove(*firmwareFs, FIRMWARE_FILE_PATH, notExistingOk);
    }

    static void logFSConfiguration();
    static void logDirTree(FS& fs, const char* dirname, const uint8_t maxDepth)
    {
        logDirTree(fs, dirname, maxDepth, 0);
    }
    static void logFilesystemTree(FS* fs, uint8_t maxDepth);

    static void logFilesystemsInformation();

private:
    static void logDirTree(FS& fs, const char* dirname, uint8_t maxDepth, uint8_t indent);
};
