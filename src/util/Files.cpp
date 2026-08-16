#include "util/Files.h"

#include "logging/Loggers.h"

bool Files::move(const String& oldPath, const String& newPath, const bool deleteIfNewExists)
{
    logger.infoln("Moving " + oldPath + " to " + newPath);

    if (LittleFS.exists(newPath))
    {
        if (deleteIfNewExists)
        {
            logger.debugln(newPath + " exists. Removing...");

            if (!LittleFS.remove(newPath))
            {
                logger.errorln("Failed to remove file. Move failed");
                return false;
            }

            logger.debugln("Removed file");
        }
        else
        {
            logger.errorln(newPath + " exists. Move failed");
            return false;
        }
    }

    if (!LittleFS.rename(oldPath, newPath))
    {
        logger.errorln("Move (rename) failed.");
        return false;
    }

    const bool removeSuccess = remove(oldPath);

    logger.logInfoOrErrorln(removeSuccess, "Move successful", "Move (remove old file) failed");

    return removeSuccess;
}

bool Files::remove(const String& path, const bool notExistingOk)
{
    if (notExistingOk && !LittleFS.exists(path)) return true;
    const bool removeSuccess = LittleFS.remove(path);
    return removeSuccess;
}
