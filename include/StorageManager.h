#pragma once

#include <FS.h>

#include "Intern.h"

class StorageManager
{
    static bool eepromIsMounted;
    static bool sspiffsIsMounted;
    static bool sdCardIsMounted;

public:
    static FS* logFileFs;
    static FS* rfidsFs;
    static FS* firmwareFs;
    static FS* gpsFs;
    static FS* consentToGPSTrackingRfidsFs;

    StorageManager() = delete; // Prevent instantiation

    static void setFS(FS& logFile, FS& rfids, FS& firmware, FS& gps, FS& consentToGPSTrackingRfids);
    static bool mountEEPROM();
    static bool mountSSPIFFS();
    static bool mountSDCard();

    static bool isSDCardInserted();

    static void saveConfigToEEPROM(Config& c);
    static bool loadConfigFromEEPROM(Config& c);
    static void resetEEPROM();

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

    static File openGPS(const char* mode, const bool create = false)
    {
        return gpsFs->open(GPS_FILE_PATH, mode, create);
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
};
