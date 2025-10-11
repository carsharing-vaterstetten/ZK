#include "RFIDs.h"

#include <FS.h>
#include "mbedtls/md5.h"
#include <ArduinoJson.h>
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

    constexpr uint8_t hashLen = 16;

    uint8_t remoteMd5Hash[hashLen];
    const int statusCode = Modem::simpleGetBin(
        REMOTE_RFID_MD5_CHECKSUM_PATH, remoteMd5Hash, hashLen, efuseMacHex, config.serverPassword);

    if (statusCode != 200)
    {
        fileLog.warningln("Unexpected status code " + String(statusCode));
        return RfidsChecksumResult::ERROR;
    }

    uint8_t fileMd5Hash[hashLen];

    File f = StorageManager::openRFIDs(FILE_READ);
    HelperUtils::md5File(f, fileMd5Hash);
    f.close();

    if (memcmp(fileMd5Hash, remoteMd5Hash, hashLen) == 0)
    {
        fileLog.infoln("Files are equal");
        return RfidsChecksumResult::FILES_ARE_EQUAL;
    }

    fileLog.infoln("Files differ");
    return RfidsChecksumResult::FILES_DIFFER;
}


bool RFIDs::downloadRfids()
{
    fileLog.infoln("Downloading remote RFIDs JSON");

    // Open HTTP stream (watchdog timeout and cleanup handled automatically by DownloadStream)
    DownloadStream http{REMOTE_RFID_PATH, *Modem::gsmClient, config.server, config.serverPort, efuseMacHex, config.serverPassword};

    if (!http)
    {
        fileLog.errorln("Failed to open stream for RFIDs download");
        return false;
    }

    // Open temp file for writing
    File file = StorageManager::openTmpRFIDs(FILE_WRITE, true);

    if (!file)
    {
        fileLog.warningln("Failed to open temp RFIDs file. Download canceled");
        return false;
    }

    // Parse JSON from stream
    fileLog.infoln("Parsing JSON stream");
    JsonDocument doc;
    const DeserializationError error = deserializeJson(doc, http);

    if (error)
    {
        fileLog.errorln("JSON parsing failed: " + String(error.c_str()));
        file.close();
        StorageManager::removeTmpRFIDs();
        return false;
    }

    const JsonArray rfids = doc.as<JsonArray>();

    fileLog.infoln("Writing " + String(rfids.size()) + " RFIDs to file");
    for (const JsonVariant rfidVariant : rfids)
    {
        const auto rfid = rfidVariant.as<uint32_t>();
        file.write(reinterpret_cast<const uint8_t*>(&rfid), sizeof(rfid));
    }

    file.close();

    fileLog.infoln("Successfully downloaded and parsed RFIDs file");

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
