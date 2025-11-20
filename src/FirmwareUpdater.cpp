#include "FirmwareUpdater.h"

#include <Update.h>

#include "Backend.h"
#include "Globals.h"
#include "LocalConfig.h"
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

void FirmwareUpdater::doUpdateIfAvailable()
{
    fileLog.infoln("Checking for firmware update");

    DownloadStream downloadStream{LATEST_FIRMWARE_DOWNLOAD_PATH "?fm_version=" FIRMWARE_VERSION};

    if (!downloadStream)
    {
        fileLog.errorln("Failed to open stream for update");
        return;
    }

    if (downloadStream.responseStatusCode() == 204)
    {
        fileLog.infoln("Already running latest firmware");
        return;
    }

    if (downloadStream.responseStatusCode() != 200)
    {
        fileLog.errorln("Unexpected status code. Failed to check for update");
        return;
    }

    fileLog.infoln("Newer version available");
    fileLog.infoln("Performing OTA update");

    const size_t updateSize = downloadStream.contentLength();
    fileLog.infoln("Update size: " + String(updateSize) + " B");

    if (!Update.begin(updateSize))
    {
        fileLog.errorln("Not enough space for OTA");
        return;
    }

    nextDownloadProgressPrint = 0;
    Update.onProgress(onDownloadProgress);

    const size_t written = Update.writeStream(downloadStream);

    if (written != updateSize)
    {
        fileLog.errorln("Write failed. Written " + String(written) + " B / " + String(updateSize) + " B");
        return;
    }

    if (!Update.end())
    {
        fileLog.errorln("Update failed: " + String(Update.getError()));
        return;
    }

    if (!Update.isFinished())
    {
        fileLog.errorln("Update not finished?");
        return;
    }

    fileLog.infoln("Update complete. Rebooting in 5 seconds...");
    delay(5000);
    ESP.restart();

    // Never reaches here
}
