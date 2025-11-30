#include "StorageManager.h"

#include "Log.h"

bool StorageManager::mountLittleFS()
{
    if (flashIsMounted) return true;
    flashIsMounted = LittleFS.begin(true);
    return flashIsMounted;
}

bool StorageManager::replaceRFIDsFileWithTmpRFIDs()
{
    const bool moveSuccess = move(TMP_RFID_FILE_PATH, RFID_FILE_PATH, true);
    fileLog.logInfoOrErrorln(moveSuccess, "RFID UIDs updated successfully", "RFID UIDs not updated");
    return moveSuccess;
}

bool StorageManager::replaceGpsUIDsFileWithTmpUIDs()
{
    const bool moveSuccess = move(TMP_RFID_FILE_PATH, GPS_TRACKING_CONSENTED_RFIDS_FILE_PATH, true);
    fileLog.logInfoOrErrorln(moveSuccess, "GPS RFID UIDs updated successfully", "GPS RFID UIDs not updated");
    return moveSuccess;
}

bool StorageManager::move(const String& oldPath, const String& newPath, const bool deleteIfNewExists = false)
{
    fileLog.infoln("Moving " + oldPath + " to " + newPath);

    if (LittleFS.exists(newPath))
    {
        if (deleteIfNewExists)
        {
            fileLog.debugln(newPath + " exists. Removing...");

            if (!LittleFS.remove(newPath))
            {
                fileLog.errorln("Failed to remove file. Move failed");
                return false;
            }

            fileLog.debugln("Removed file");
        }
        else
        {
            fileLog.errorln(newPath + " exists. Move failed");
            return false;
        }
    }

    if (!LittleFS.rename(oldPath, newPath))
    {
        fileLog.errorln("Move (rename) failed.");
        return false;
    }

    const bool removeSuccess = remove(oldPath);

    fileLog.logInfoOrErrorln(removeSuccess, "Move successful", "Move (remove old file) failed");

    return removeSuccess;
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

void StorageManager::logFilesystemsInformation() const
{
    if (flashIsMounted)
    {
        const size_t flashUtilized = LittleFS.usedBytes();
        const size_t flashTotal = LittleFS.totalBytes();
        fileLog.infoln("Flash usage: " + String(flashUtilized) + " B / " + String(flashTotal) + " B");
    }
}
