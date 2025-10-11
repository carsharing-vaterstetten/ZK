#include "FirmwareUpdater.h"

#include <Update.h>

#include "Backend.h"
#include "Globals.h"
#include "HelperUtils.h"
#include "Modem.h"
#include "StorageManager.h"

#define UPDATE_COMMAND U_FLASH

bool FirmwareUpdater::downloadAndPerformUpdate()
{
    fileLog.infoln("Performing OTA update");

    File downloadFile = StorageManager::openFirmware(FILE_WRITE, true);

    if (!downloadFile)
    {
        fileLog.errorln("Failed to open firmware file");
        return false;
    }

    const DownloadResult downloadResult = Modem::downloadFile(
        LATEST_FIRMWARE_DOWNLOAD_PATH, downloadFile, modemIMEI, config.serverPassword);
    downloadFile.close(); // Just to make sure

    switch (downloadResult)
    {
    case DownloadResult::FAILED_TO_OPEN_STREAM:
        return false;
    case DownloadResult::SUCCESS:
        break;
    }

    File updateFile = StorageManager::openFirmware(FILE_READ);

    const size_t fileSize = updateFile.size();

    fileLog.infoln("Update size: " + String(fileSize) + " B");

    if (!Update.begin(fileSize, UPDATE_COMMAND))
    {
        fileLog.errorln("Not enough space for OTA");
        return false;
    }

    const size_t written = Update.writeStream(updateFile);

    if (written != fileSize)
    {
        fileLog.errorln("Write failed. Written " + String(written) + " B / " + String(fileSize) + " B");
        return false;
    }

    if (!Update.end())
    {
        fileLog.errorln("Update failed: " + String(Update.getError()));
        return false;
    }

    if (Update.isFinished())
    {
        fileLog.infoln("Update complete. Rebooting in 5 seconds...");
        delay(5000);
        ESP.restart();
        return true;
    }

    fileLog.errorln("Update not finished?");
    return false;
}

long FirmwareUpdater::getLatestFirmwareSize()
{
    fileLog.infoln("Checking update size...");

    String respSize;
    Modem::simpleGet(LATEST_FIRMWARE_SIZE_ENDPOINT, &respSize, modemIMEI, config.serverPassword);
    const long latestFirmwareSize = respSize.toInt();

    fileLog.infoln("Update size: " + String(latestFirmwareSize) + " B");

    if (latestFirmwareSize <= 0)
    {
        fileLog.warningln("Invalid update size");
    }

    return latestFirmwareSize;
}

FirmwareUpdateCheckResult FirmwareUpdater::checkForFirmwareUpdate()
{
    fileLog.infoln("Checking for firmware update");

    String updateAvailability;

    const int respStatus = Modem::simpleGet(LATEST_FIRMWARE_IS_NEWER_ENDPOINT "?fm_version=" FIRMWARE_VERSION,
                                            &updateAvailability, modemIMEI, config.serverPassword);

    if (respStatus != 200)
    {
        fileLog.warningln(
            "Failed to check for update: Response status: " + String(respStatus));
        return FirmwareUpdateCheckResult::ERROR;
    }

    if (updateAvailability == "true")
    {
        fileLog.infoln("Newer version available");
        return FirmwareUpdateCheckResult::UPDATE_AVAILABLE;
    }
    if (updateAvailability == "false")
    {
        fileLog.infoln("Already running latest firmware");
        return FirmwareUpdateCheckResult::NO_UPDATE_AVAILABLE;
    }

    fileLog.errorln("Unknown response: " + updateAvailability);
    return FirmwareUpdateCheckResult::UNKNOWN_RESPONSE;
}

bool FirmwareUpdater::doUpdateIfAvailable()
{
    if (checkForFirmwareUpdate() == FirmwareUpdateCheckResult::UPDATE_AVAILABLE)
    {
        return downloadAndPerformUpdate();
    }
    return false;
}
