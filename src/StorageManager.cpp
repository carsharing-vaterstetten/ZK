#include "StorageManager.h"

#include <EEPROM.h>
#include <SD.h>
#include <SPIFFS.h>

#include "Config.h"
#include "Globals.h"
#include "HardwareManager.h"
#include "Intern.h"

bool StorageManager::eepromIsMounted = false;
bool StorageManager::sspiffsIsMounted = false;
bool StorageManager::sdCardIsMounted = false;

FS* StorageManager::logFileFs = nullptr;
FS* StorageManager::rfidsFs = nullptr;
FS* StorageManager::firmwareFs = nullptr;
FS* StorageManager::gpsFs = nullptr;
FS* StorageManager::consentToGPSTrackingRfidsFs = nullptr;

void StorageManager::setFS(FS& logFile, FS& rfids, FS& firmware, FS& gps, FS& consentToGPSTrackingRfids)
{
    logFileFs = &logFile;
    rfidsFs = &rfids;
    firmwareFs = &firmware;
    gpsFs = &gps;
    consentToGPSTrackingRfidsFs = &consentToGPSTrackingRfids;
}

bool StorageManager::mountEEPROM()
{
    if (eepromIsMounted) return true;
    eepromIsMounted = EEPROM.begin(sizeof(Config));
    return eepromIsMounted;
}

bool StorageManager::mountSSPIFFS()
{
    if (sspiffsIsMounted) return true;
    sspiffsIsMounted = SPIFFS.begin(true);
    return sspiffsIsMounted;
}

bool StorageManager::mountSDCard()
{
    if (sdCardIsMounted) return true;
    HardwareManager::ensureSDSPIInitialized();
    sdCardIsMounted = SD.begin(SD_CS, *HardwareManager::sdSpi);
    return sdCardIsMounted;
}

bool StorageManager::isSDCardConnected()
{
    // Don't worry about these errors:
    // [ 28611][E][sd_diskio.cpp:806] sdcard_mount(): f_mount failed: (3) The physical drive cannot work
    // [ 29120][E][sd_diskio.cpp:126] sdSelectCard(): Select Failed
    HardwareManager::ensureSDSPIInitialized();
    SD.end();
    sdCardIsMounted = SD.begin(SD_CS, *HardwareManager::sdSpi);
    return sdCardIsMounted && SD.cardType() != CARD_NONE;
}

void StorageManager::saveConfigToEEPROM(Config& c)
{
    c.version = CONFIG_VERSION;
    fileLog.infoln("Saving config to EEPROM");
    EEPROM.put(CONFIG_START_ADDRESS, c);
    const bool commitSuccess = EEPROM.commit();
    fileLog.logInfoOrErrorln(commitSuccess, "Commiting config to EEPROM succeeded",
                             "Commiting config to EEPROM failed");
}

bool StorageManager::loadConfigFromEEPROM(Config& c)
{
    EEPROM.get(CONFIG_START_ADDRESS, c);
    return c.version == CONFIG_VERSION;
}

// Funktion zum Zurücksetzen des EEPROM
void StorageManager::resetEEPROM()
{
    Config emptyConfig{};
    emptyConfig.version = 0xFF;
    EEPROM.put(CONFIG_START_ADDRESS, emptyConfig);
    EEPROM.commit();
    fileLog.infoln("EEPROM has been reset");
}

bool StorageManager::remove(FS& fs, const String& path, const bool notExistingOk)
{
    if (notExistingOk && !fs.exists(path)) return true;
    const bool removeSuccess = fs.remove(path);
    return removeSuccess;
}
