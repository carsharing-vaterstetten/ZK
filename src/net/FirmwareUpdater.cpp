#include "net/FirmwareUpdater.h"

#include <Update.h>

#include "system/SystemManager.h"
#include "net/ApiClient.h"
#include "config/Backend.h"
#include "hal/Modem.h"
#include "config/Intern.h"
#include "logging/Loggers.h"

#define UPDATE_COMMAND U_FLASH

size_t nextDownloadProgressPrint = 0;

void onDownloadProgress(const size_t progress, const size_t total)
{
    if (progress >= nextDownloadProgressPrint)
    {
        logger.debugln("Downloaded " + String(progress) + " B of the Update");

        nextDownloadProgressPrint = progress + total / 10;
    }
}

ApiResponse checkForUpdate(ApiClient& api)
{
    logger.infoln("Checking for firmware update");
    const HttpRequest req = HttpRequest::get(LATEST_FIRMWARE_DOWNLOAD_PATH "?fm_version=" FIRMWARE_VERSION);
    const ApiResponse resp = api.makeRequest(req);
    if (!resp.valid) logger.errorln("Request failed");
    return resp;
}

bool isUpdateAvailable(const ApiResponse& resp)
{
    logger.infoln("Response code: " + String(resp.responseCode));
    if (resp.responseCode == 204)
    {
        logger.infoln("Already running latest firmware");
        return false;
    }
    if (resp.responseCode != 200)
    {
        logger.errorln("Unexpected status code. Failed to check for update");
        return false;
    }
    logger.infoln("Newer version available");
    return true;
}

bool prepareUpdate(ApiClient& api, const ApiResponse& resp)
{
    const size_t updateSize = resp.bodyLength;
    logger.infoln("Update size: " + String(updateSize) + " B");

    if (!Update.begin(updateSize))
    {
        logger.errorln("Not enough space for OTA");
        return false;
    }

    if (resp.headers.count("x-md5"))
    {
        if (!Update.setMD5(resp.headers.at("x-md5").c_str()))
        {
            logger.errorln("Failed to set MD5. Update canceled");
            return false;
        }
    }
    else
    {
        logger.warningln("MD5 header not found. Skipping MD5 verification.");
    }

    nextDownloadProgressPrint = 0;

    // writeFirmware() below reads via Update.writeStream(), not api.fetch(), so
    // this is the only place progress from the download actually surfaces —
    // mirror it into the ApiClient state StartupTask polls for the LED bar.
    api.reportExternalProgress(ApiClientState::Downloading, 0, updateSize);

    Update.onProgress([&api](const size_t progress, const size_t total)
    {
        onDownloadProgress(progress, total);
        api.reportExternalProgress(ApiClientState::Downloading, total - progress, total);
    });

    return true;
}

bool writeFirmware(const ApiResponse& resp)
{
    const size_t written = Update.writeStream(resp.body);
    if (written != resp.bodyLength)
    {
        logger.errorln("Write failed. Written " + String(written) + " B / " + String(resp.bodyLength) + " B");
        return false;
    }
    return true;
}

bool finalizeUpdate()
{
    if (!Update.end())
    {
        logger.errorln("Update failed: " + String(Update.getError()));
        return false;
    }
    if (!Update.isFinished())
    {
        logger.errorln("Update not finished?");
        return false;
    }
    return true;
}


void FirmwareUpdater::doUpdateIfAvailable(ApiClient& api)
{
    const ApiResponse resp = checkForUpdate(api);
    if (!resp.valid || !isUpdateAvailable(resp)) return;
    logger.infoln("Performing OTA update");

    if (!prepareUpdate(api, resp)) return;

    const bool writeSuccess = writeFirmware(resp);
    api.endExternalTransfer(); // stop reporting Downloading whether it succeeded or not

    if (!writeSuccess) return;
    if (!finalizeUpdate()) return;

    logger.infoln("Update complete. Requesting restart");
    SystemManager::TriggerSystemHotRestart();
}
