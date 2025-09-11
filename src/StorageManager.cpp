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

void StorageManager::setFS(FS& logFile, FS& rfids, FS& firmware)
{
    logFileFs = &logFile;
    rfidsFs = &rfids;
    firmwareFs = &firmware;
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

bool StorageManager::isSDCardInserted()
{
    return config.preferSDCard; // This must do it for now
    // TODO This will just halt until watchdog reset when no sd card is inserted...
    // For some reason writing to a file breaks when the mountpoint is passed in a variable??? I tried it in a clean project and there it works???
    // for unknown reasons sdmmc_frequency must be 10000 https://github.com/espressif/esp-idf/issues/10194
    // sdCardIsMounted = SD_MMC.begin("/sdcard", true, true, 10000);
    // Will quietly fail when no SD card connected and program execution stops.
    // sdCardIsMounted = SD_MMC.cardType() != CARD_NONE;
    // return sdCardIsMounted;
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
