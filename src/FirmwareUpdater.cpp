#include "FirmwareUpdater.h"

#include <Update.h>

#include "Backend.h"
#include "Globals.h"
#include "Modem.h"
#include "StorageManager.h"

#define UPDATE_COMMAND U_FLASH

size_t nextDownloadProgressPrint = 0;

void onDownloadProgress(const size_t progress, const size_t total)
{
    if (progress >= nextDownloadProgressPrint)
    {
        fileLog.debugln("Downloaded " + String(progress) + " B of the Update");

        nextDownloadProgressPrint = progress + total / 10;
    }
}

bool FirmwareUpdater::performUpdate()
{
    fileLog.infoln("Performing OTA update");

    DownloadStream downloadStream{
        LATEST_FIRMWARE_DOWNLOAD_PATH, *Modem::gsmClient, config.server, config.serverPort, modemIMEI,
        config.serverPassword
    };

    if (!downloadStream)
    {
        fileLog.errorln("Failed to open stream for download");
        return false;
    }

    const size_t updateSize = downloadStream.contentLength();
    fileLog.infoln("Update size: " + String(updateSize) + " B");

    if (!Update.begin(updateSize))
    {
        fileLog.errorln("Not enough space for OTA");
        return false;
    }

    nextDownloadProgressPrint = 0;
    Update.onProgress(onDownloadProgress);

    const size_t written = Update.writeStream(downloadStream);

    if (written == updateSize)
    {
        fileLog.infoln("Written : " + String(written) + " successfully");
    }
    else
    {
        fileLog.errorln("Write failed. Written " + String(written) + " B / " + String(updateSize) + " B");
        return false;
    }

    if (!Update.end())
    {
        fileLog.errorln("Update failed: " + String(Update.getError()));
        return false;
    }

    if (!Update.isFinished())
    {
        fileLog.errorln("Update not finished?");
        return false;
    }

    fileLog.infoln("Update complete. Rebooting in 5 seconds...");
    delay(5000);
    ESP.restart();

    // Never reaches here
    return true;
}

uint32_t FirmwareUpdater::getLatestFirmwareSize()
{
    fileLog.infoln("Checking update size...");

    String respSize;
    Modem::simpleGet(LATEST_FIRMWARE_SIZE_ENDPOINT, &respSize, modemIMEI, config.serverPassword);
    const uint32_t latestFirmwareSize = respSize.toInt();

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
        return performUpdate();
    }
    return false;
}
