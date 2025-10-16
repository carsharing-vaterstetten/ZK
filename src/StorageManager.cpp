#include "StorageManager.h"

#include "Globals.h"

bool StorageManager::flashIsMounted = false;

bool StorageManager::mountLittleFS()
{
    if (flashIsMounted) return true;
    flashIsMounted = LittleFS.begin(true);
    return flashIsMounted;
}

bool StorageManager::remove(const String& path, const bool notExistingOk)
{
    if (notExistingOk && !LittleFS.exists(path)) return true;
    const bool removeSuccess = LittleFS.remove(path);
    return removeSuccess;
}

void StorageManager::logFilesystemTree(const uint8_t maxDepth)
{
    Serial.println("/");
    logDirTree("/", maxDepth, 1);
}

void StorageManager::logDirTree(const char* dirname, const uint8_t maxDepth, const uint8_t indent)
{
    if (maxDepth <= 0) return;

    File f = LittleFS.open(dirname);
    File nextFile = f.openNextFile();


    while (nextFile)
    {
        for (uint8_t i = 0; i < indent; ++i) Serial.print("  ");

        if (nextFile.isDirectory())
        {
            Serial.println(String(nextFile.name()) + "/");
            logDirTree(nextFile.path(), maxDepth - 1, indent + 1);
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
    if (flashIsMounted)
    {
        const size_t flashUtilized = LittleFS.usedBytes();
        const size_t flashTotal = LittleFS.totalBytes();
        fileLog.infoln("Flash usage: " + String(flashUtilized) + " B / " + String(flashTotal) + " B");
    }
}
