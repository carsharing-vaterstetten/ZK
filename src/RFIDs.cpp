#include "RFIDs.h"

#include <FS.h>
#include "mbedtls/md5.h"
#include "Backend.h"
#include "Globals.h"
#include "HelperUtils.h"
#include "Modem.h"
#include "StorageManager.h"

bool RFIDs::isRegisteredRFID(const uint32_t rfid)
{
    File file = StorageManager::openRFIDs(FILE_READ);

    if (!file)
    {
        fileLog.errorln("Failed to open rfid file for reading");
        return false;
    }

    const size_t rfidsCount = file.size() / 4;
    uint32_t buffer;

    for (int i = 0; i < rfidsCount; i++)
    {
        file.read(reinterpret_cast<uint8_t*>(&buffer), 4);
        if (buffer == rfid)
        {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

RfidsChecksumResult RFIDs::compareChecksums()
{
    fileLog.infoln("Comparing RFID file checksums");

    if (!StorageManager::rfidsFileExists())
    {
        fileLog.infoln("Local RFIDs file does not exist");
        return RfidsChecksumResult::LOCAL_FILE_DOES_NOT_EXIST;
    }

    String response;
    const int statusCode = Modem::simpleGet(REMOTE_RFID_MD5_CHECKSUM_PATH, &response, efuseMacHex, config.password);

    if (statusCode != 200)
    {
        fileLog.warningln("Unexpected status code " + String(statusCode));
        return RfidsChecksumResult::ERROR;
    }

    uint8_t md5Hash[16];

    File f = StorageManager::openRFIDs(FILE_READ);
    HelperUtils::md5File(f, md5Hash);
    f.close();

    const String md5Hex = HelperUtils::md5ToHex(md5Hash);

    if (response == "\"" + md5Hex + "\"")
    {
        fileLog.infoln("Files are equal");
        return RfidsChecksumResult::FILES_ARE_EQUAL;
    }

    fileLog.infoln("Files differ");
    return RfidsChecksumResult::FILES_DIFFER;
}


bool RFIDs::downloadRfids()
{
    fileLog.infoln("Downloading remote RFIDs file");

    File file = StorageManager::openTmpRFIDs(FILE_WRITE, true);

    if (!file)
    {
        fileLog.warningln("Failed to open temp RFIDs file. Download canceled");
        return false;
    }

    const DownloadResult downloadResult = Modem::downloadFile(REMOTE_RFID_PATH, file, efuseMacHex, config.password);

    switch (downloadResult)
    {
    case DownloadResult::HTTP_REQUEST_ERROR:
    case DownloadResult::UNEXPECTED_STATUS_CODE:
        fileLog.errorln("RFIDs file download failed");
        StorageManager::removeTmpRFIDs();
        return false;
    case DownloadResult::SUCCESS:
        break;
    }

    fileLog.infoln("Successfully downloaded RFIDs file");

    const bool removeOldSuccess = StorageManager::removeRFIDs();
    fileLog.logInfoOrWarningln(removeOldSuccess, "Removed old RFIDs file successfully",
                               "Failed to remove old RFIDs file");

    const bool renameSuccess = StorageManager::rfidsFs->rename(TMP_RFID_FILE_PATH, RFID_FILE_PATH);

    fileLog.logInfoOrWarningln(renameSuccess, "Successfully renamed RFIDs file",
                               "Failed to rename RFIDs file. RFIDs not updated");

    StorageManager::removeTmpRFIDs();

    return renameSuccess;
}

void RFIDs::downloadRfidsIfChanged()
{
    switch (compareChecksums())
    {
    case RfidsChecksumResult::ERROR:
    case RfidsChecksumResult::FILES_ARE_EQUAL:
        break;
    case RfidsChecksumResult::FILES_DIFFER:
    case RfidsChecksumResult::LOCAL_FILE_DOES_NOT_EXIST:
        downloadRfids();
        break;
    }
}

bool RFIDs::downloadGPSTrackingConsentedRFIDs()
{
    fileLog.infoln("Downloading remote RFIDs that consent to GPS tracking file");

    File file = StorageManager::openTmpRFIDs(FILE_WRITE, true);

    if (!file)
    {
        fileLog.warningln("Failed to open temp RFIDs file. Download canceled");
        return false;
    }

    const DownloadResult downloadResult = Modem::downloadFile(
        REMOTE_GPS_TRACKING_CONSENTED_RFIDS_PATH, file, efuseMacHex, config.password);

    switch (downloadResult)
    {
    case DownloadResult::HTTP_REQUEST_ERROR:
    case DownloadResult::UNEXPECTED_STATUS_CODE:
        fileLog.errorln("Download failed");
        StorageManager::removeTmpRFIDs();
        return false;
    case DownloadResult::SUCCESS:
        break;
    }

    fileLog.infoln("Successfully downloaded file");

    const bool removeOldSuccess = StorageManager::remove(*StorageManager::consentToGPSTrackingRfidsFs,
                                                         GPS_TRACKING_CONSENTED_RFIDS_FILE_PATH);

    fileLog.logInfoOrWarningln(removeOldSuccess, "Removed old RFIDs file successfully",
                               "Failed to remove old RFIDs file");

    const bool renameSuccess = StorageManager::rfidsFs->rename(
        TMP_RFID_FILE_PATH, GPS_TRACKING_CONSENTED_RFIDS_FILE_PATH);

    fileLog.logInfoOrWarningln(renameSuccess, "Successfully renamed file",
                               "Failed to rename file. RFIDs not updated");

    StorageManager::removeTmpRFIDs();

    return renameSuccess;
}


bool RFIDs::RFIDConsentsToGPSTrackingTest(const uint32_t rfid)
{
    File file = StorageManager::consentToGPSTrackingRfidsFs->open(GPS_TRACKING_CONSENTED_RFIDS_FILE_PATH, FILE_READ);

    if (!file)
    {
        fileLog.errorln("Failed to open GPS consent RFIDs file for reading");
        return false;
    }

    const size_t rfidsCount = file.size() / 4;
    uint32_t buffer;

    for (int i = 0; i < rfidsCount; i++)
    {
        file.read(reinterpret_cast<uint8_t*>(&buffer), 4);
        if (buffer == rfid)
        {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}
