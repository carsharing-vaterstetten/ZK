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

    const DownloadResult downloadResult = Modem::downloadFile(LATEST_FIRMWARE_DOWNLOAD_PATH, downloadFile);
    downloadFile.close(); // Just to make sure

    switch (downloadResult)
    {
    case DownloadResult::HTTP_REQUEST_ERROR:
    case DownloadResult::UNEXPECTED_STATUS_CODE:
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
    Modem::simpleGet(LATEST_FIRMWARE_SIZE_ENDPOINT, &respSize);
    const long latestFirmwareSize = respSize.toInt();

    fileLog.infoln("Update size: " + String(latestFirmwareSize) + " B");

    if (latestFirmwareSize <= 0)
    {
        fileLog.warningln("Invalid update size");
    }

    return latestFirmwareSize;
}


bool FirmwareUpdater::doFirmwareUpdateWithWatchdog()
{
    // Increase the HW Watchdog timeout to allow for longer OTA updates

    esp_err_t hwd_err = HelperUtils::setWatchdog(HW_WATCHDOG_OTA_UPDATE_TIMEOUT);
    if (hwd_err != ESP_OK)
    {
        fileLog.errorln("Failed to set HW Watchdog for OTA update. OTA update will not be performed!");
        return false;
    }

    const bool success = downloadAndPerformUpdate();

    // Reset the HW Watchdog timeout to the default value regardless of whether an update was performed or not
    hwd_err = HelperUtils::setWatchdog(HW_WATCHDOG_DEFAULT_TIMEOUT);
    if (hwd_err != ESP_OK)
    {
        fileLog.warningln("Failed to reset HW Watchdog timeout after OTA update.");
    }

    return success;
}

FirmwareUpdateCheckResult FirmwareUpdater::checkForFirmwareUpdate()
{
    fileLog.infoln("Checking for firmware update");

    String updateAvailability;

    const int respStatus = Modem::simpleGet(LATEST_FIRMWARE_IS_NEWER_ENDPOINT "?fm_version=" FIRMWARE_VERSION,
                                            &updateAvailability);

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
        return doFirmwareUpdateWithWatchdog();
    }
    return false;
}
