#include "LittleFSHelper.h"

#include "shared/Globals.h"

bool LittleFSHelper::move(const String& oldPath, const String& newPath, const bool deleteIfNewExists = false)
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

bool LittleFSHelper::remove(const String& path, const bool notExistingOk)
{
    if (notExistingOk && !LittleFS.exists(path)) return true;
    const bool removeSuccess = LittleFS.remove(path);
    return removeSuccess;
}

void LittleFSHelper::logFilesystemsInformation()
{
    const size_t flashUtilized = LittleFS.usedBytes();
    const size_t flashTotal = LittleFS.totalBytes();
    fileLog.infoln("Flash usage: " + String(flashUtilized) + " B / " + String(flashTotal) + " B");
}
