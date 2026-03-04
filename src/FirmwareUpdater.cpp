#include "FirmwareUpdater.h"

#include <Update.h>

#include "Api.h"
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

ApiResponse checkForUpdate()
{
    fileLog.infoln("Checking for firmware update");
    const HttpRequest req = HttpRequest::get(LATEST_FIRMWARE_DOWNLOAD_PATH "?fm_version=" FIRMWARE_VERSION);
    const ApiResponse resp = api.makeRequest(req);
    if (!resp.valid) fileLog.errorln("Request failed");
    return resp;
}

bool isUpdateAvailable(const ApiResponse& resp)
{
    fileLog.infoln("Response code: " + String(resp.responseCode));
    if (resp.responseCode == 204)
    {
        fileLog.infoln("Already running latest firmware");
        return false;
    }
    if (resp.responseCode != 200)
    {
        fileLog.errorln("Unexpected status code. Failed to check for update");
        return false;
    }
    fileLog.infoln("Newer version available");
    return true;
}

bool prepareUpdate(const ApiResponse& resp)
{
    const size_t updateSize = resp.bodyLength;
    fileLog.infoln("Update size: " + String(updateSize) + " B");

    if (!Update.begin(updateSize))
    {
        fileLog.errorln("Not enough space for OTA");
        return false;
    }

    if (resp.headers.count("x-md5"))
    {
        if (!Update.setMD5(resp.headers.at("x-md5").c_str()))
        {
            fileLog.errorln("Failed to set MD5. Update canceled");
            return false;
        }
    }
    else
    {
        fileLog.warningln("MD5 header not found. Skipping MD5 verification.");
    }

    nextDownloadProgressPrint = 0;
    Update.onProgress(onDownloadProgress);
    return true;
}

bool writeFirmware(const ApiResponse& resp)
{
    const size_t written = Update.writeStream(resp.body);
    if (written != resp.bodyLength)
    {
        fileLog.errorln("Write failed. Written " + String(written) + " B / " + String(resp.bodyLength) + " B");
        return false;
    }
    return true;
}

bool finalizeUpdate()
{
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
    return true;
}


void FirmwareUpdater::doUpdateIfAvailable()
{
    const ApiResponse resp = checkForUpdate();
    if (!resp.valid || !isUpdateAvailable(resp)) return;
    fileLog.infoln("Performing OTA update");

    if (!prepareUpdate(resp)) return;
    if (!writeFirmware(resp)) return;
    if (!finalizeUpdate()) return;

    fileLog.infoln("Update complete. Rebooting in 5 seconds...");
    delay(5000);
    ESP.restart();
}
