#include "RFIDs.h"

#include "mbedtls/md5.h"
#include <ArduinoJson.h>
#include "Backend.h"
#include "Globals.h"
#include "HelperUtils.h"
#include "LocalConfig.h"
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

void RFIDs::generateChecksum(uint8_t out[16])
{
    if (!StorageManager::rfidsFileExists())
    {
        fileLog.infoln("Local RFIDs file does not exist");
        constexpr uint8_t empty_md5[16] = {
            0xd4, 0x1d, 0x8c, 0xd9,
            0x8f, 0x00, 0xb2, 0x04,
            0xe9, 0x80, 0x09, 0x98,
            0xec, 0xf8, 0x42, 0x7e
        };
        memcpy(out, empty_md5, 16);
        return;
    }

    File f = StorageManager::openRFIDs(FILE_READ);
    HelperUtils::md5File(f, out);
    f.close();
}

void RFIDs::downloadRfidsIfChanged()
{
    fileLog.infoln("Downloading remote RFIDs JSON");

    // Open HTTP stream (watchdog timeout and cleanup handled automatically by DownloadStream)

    uint8_t md5Checksum[16];
    generateChecksum(md5Checksum);

    DownloadStream http{REMOTE_RFID_PATH, HelperUtils::toBase64(md5Checksum, 16)};

    if (!http)
    {
        fileLog.errorln("Failed to open stream for RFIDs download");
        return;
    }

    const uint16_t responseCode = http.responseStatusCode();

    if (responseCode == 304)
    {
        fileLog.infoln("Server and local RFID UIDs are the same");
        return;
    }

    if (responseCode != 200)
    {
        fileLog.errorln("Unexpected response code. Using old RFID UIDs");
        return;
    }

    // Open temp file for writing
    File file = StorageManager::openTmpRFIDs(FILE_WRITE, true);

    if (!file)
    {
        fileLog.warningln("Failed to open temp RFIDs file. Download canceled");
        return;
    }

    http.setTimeout(100000); // [ms] = 100s. Necessary for JSON parsing

    // Parse JSON from stream
    fileLog.infoln("Parsing JSON stream");
    JsonDocument doc;
    const DeserializationError error = deserializeJson(doc, http);

    if (error)
    {
        fileLog.errorln("JSON parsing failed: " + String(error.c_str()));
        file.close();
        StorageManager::removeTmpRFIDs();
        return;
    }

    const auto rfids = doc.as<JsonArray>();

    fileLog.infoln("Writing " + String(rfids.size()) + " RFIDs to file");
    for (const JsonVariant rfidVariant : rfids)
    {
        const auto rfid = rfidVariant.as<uint32_t>();
        file.write(reinterpret_cast<const uint8_t*>(&rfid), sizeof(rfid));
    }

    file.close();

    fileLog.infoln("Successfully downloaded and parsed RFIDs file");

    StorageManager::replaceRFIDsFileWithTmpRFIDs();
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

    const DownloadResult downloadResult = modem.downloadFile(
        REMOTE_GPS_TRACKING_CONSENTED_RFIDS_PATH, file, modemIMEI, config.serverPassword);

    switch (downloadResult)
    {
    case DownloadResult::FAILED_TO_OPEN_STREAM:
    case DownloadResult::UNEXPECTED_STATUS_CODE:
        fileLog.errorln("Download failed");
        StorageManager::removeTmpRFIDs();
        return false;
    case DownloadResult::SUCCESS:
        break;
    }

    fileLog.infoln("Successfully downloaded file");

    return StorageManager::replaceGpsUIDsFileWithTmpUIDs();
}


bool RFIDs::RFIDConsentsToGPSTrackingTest(const uint32_t rfid)
{
    File file = StorageManager::openGpsRFIDs(FILE_READ);

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
