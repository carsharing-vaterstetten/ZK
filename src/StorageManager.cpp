#include "StorageManager.h"

#include <SPIFFS.h>

#include "Globals.h"
#include "HardwareManager.h"
#include "Intern.h"

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

bool StorageManager::mountSSPIFFS()
{
    if (sspiffsIsMounted) return true;
    sspiffsIsMounted = SPIFFS.begin(true);
    return sspiffsIsMounted;
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
