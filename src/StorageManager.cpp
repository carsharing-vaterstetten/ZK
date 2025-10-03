#include "StorageManager.h"

#include <EEPROM.h>
#include <SPIFFS.h>

#include "Config.h"
#include "Globals.h"
#include "HardwareManager.h"
#include "Intern.h"

bool StorageManager::eepromIsMounted = false;
bool StorageManager::sspiffsIsMounted = false;

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

String fsName(const FS* fs)
{
    if (fs == &SPIFFS) return "SPIFFS";
    return "UNKNOWN";
}

void StorageManager::logFSConfiguration()
{
    String msg = "Filesystem configuration: " LOG_FILE_PATH ": " + fsName(logFileFs);
    msg += ", " RFID_FILE_PATH ": " + fsName(rfidsFs);
    msg += ", " FIRMWARE_FILE_PATH ": " + fsName(firmwareFs);
    fileLog.infoln(msg);
}

void StorageManager::logFilesystemTree(FS* fs, const uint8_t maxDepth)
{
    Serial.println(fsName(fs) + "/");
    logDirTree(*fs, "/", maxDepth, 1);
}

void StorageManager::logDirTree(FS& fs, const char* dirname, const uint8_t maxDepth, const uint8_t indent)
{
    if (maxDepth <= 0) return;

    File f = fs.open(dirname);
    File nextFile = f.openNextFile();


    while (nextFile)
    {
        for (uint8_t i = 0; i < indent; ++i) Serial.print("  ");

        if (nextFile.isDirectory())
        {
            Serial.println(String(nextFile.name()) + "/");
            logDirTree(fs, nextFile.path(), maxDepth - 1, indent + 1);
        }
        else
        {
            Serial.println(String(nextFile.name()) + " (" + nextFile.size() + " B)");
        }

        nextFile = f.openNextFile();
    }

    f.close();
    nextFile.close();
}

void StorageManager::logFilesystemsInformation()
{
    if (sspiffsIsMounted)
    {
        const size_t flashUtilized = SPIFFS.usedBytes();
        const size_t flashTotal = SPIFFS.totalBytes();
        fileLog.infoln("SPIFFS: usage: " + String(flashUtilized) + " B / " + String(flashTotal) + " B");
    }
}
