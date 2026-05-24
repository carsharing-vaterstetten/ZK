#include "RFIDs.h"

#include "mbedtls/md5.h"
#include <ArduinoJson.h>
#include "config/Backend.h"
#include "Globals.h"
#include "HelperUtils.h"
#include "LittleFSHelper.h"
#include "config/Intern.h"

std::vector<uint32_t> rfids;

bool RFIDs::isRegisteredRFID(const uint32_t rfid)
{
    return std::binary_search(rfids.begin(), rfids.end(), rfid);
}

/// Call this function to load the UIDs into RAM to be able to check if a UID is registered.
/// So always call this on startup and after the file changed on the disk (e.g. after remote download)
bool RFIDs::load()
{
    if (!LittleFS.exists(RFID_FILE_PATH))
    {
        fileLog.errorln("Failed to check UID, file does not exist");
        return false;
    }

    File f = LittleFS.open(RFID_FILE_PATH, FILE_READ);

    if (!f)
    {
        fileLog.errorln("Failed to open rfid file for reading");
        return false;
    }

    constexpr uint uidSize = sizeof(uint32_t);
    const size_t uidsCount = f.size() / uidSize;

    rfids.clear();
    rfids.resize(uidsCount);
    f.read(reinterpret_cast<uint8_t*>(rfids.data()), uidsCount * sizeof(uint32_t));

    fileLog.debugln("Sorting RFIDs...");

    std::sort(rfids.begin(), rfids.end()); // it has to be sorted for binary search

    fileLog.infoln(
        "Loaded and sorted " + String(rfids.size()) + " UIDs (consumes " + rfids.capacity() * uidSize + " B)");

    return true;
}

void generateChecksum(uint8_t out[16])
{
    if (!LittleFS.exists(RFID_FILE_PATH))
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

    File f = LittleFS.open(RFID_FILE_PATH, FILE_READ);
    HelperUtils::md5File(f, out);
    f.close();
}

void RFIDs::downloadRfidsIfChanged(const ApiClient& api)
{
    fileLog.infoln("Downloading remote RFIDs JSON");

    uint8_t md5Checksum[16];
    generateChecksum(md5Checksum);
    String base64Checksum = HelperUtils::toBase64(md5Checksum, 16);

    const HttpRequest req = HttpRequest::get(REMOTE_RFID_PATH, {{"if-none-match", base64Checksum}});
    const ApiResponse resp = api.makeRequest(req, true);

    if (!resp.valid)
    {
        fileLog.errorln("Request failed");
        return;
    }

    fileLog.infoln("Response code: " + String(resp.responseCode));

    if (resp.responseCode == 304)
    {
        fileLog.infoln("Server and local RFID UIDs are the same");
        return;
    }

    if (resp.responseCode != 200)
    {
        fileLog.errorln("Unexpected response code. Using old RFID UIDs");
        return;
    }

    // Open temp file for writing
    File file = LittleFS.open(TMP_RFID_FILE_PATH,FILE_WRITE, true);

    if (!file)
    {
        fileLog.warningln("Failed to open temp RFIDs file. Download canceled");
        return;
    }

    resp.body.setTimeout(100000); // [ms] = 100s. Necessary for JSON parsing

    // Parse JSON from stream
    fileLog.infoln("Parsing JSON stream");
    JsonDocument doc;
    const DeserializationError error = deserializeJson(doc, resp.body);

    if (error)
    {
        fileLog.errorln("JSON parsing failed: " + String(error.c_str()));
        file.close();
        LittleFSHelper::remove(TMP_RFID_FILE_PATH);
        return;
    }

    const auto rfidsArr = doc.as<JsonArray>();

    fileLog.infoln("Writing " + String(rfidsArr.size()) + " RFIDs to file");
    for (const JsonVariant rfidVariant : rfidsArr)
    {
        const auto rfid = rfidVariant.as<uint32_t>();
        file.write(reinterpret_cast<const uint8_t*>(&rfid), sizeof(rfid));
    }

    file.close();

    fileLog.infoln("Successfully downloaded and parsed RFIDs file");

    const bool moveSuccess = LittleFSHelper::move(TMP_RFID_FILE_PATH, RFID_FILE_PATH, true);
    fileLog.logInfoOrErrorln(moveSuccess, "RFID UIDs updated successfully", "RFID UIDs not updated");
}

bool RFIDs::downloadGPSTrackingConsentedRFIDs(const ApiClient& api)
{
    fileLog.infoln("Downloading remote RFIDs that consent to GPS tracking file");

    File file = LittleFS.open(TMP_RFID_FILE_PATH, FILE_WRITE, true);

    if (!file)
    {
        fileLog.warningln("Failed to open temp RFIDs file. Download canceled");
        return false;
    }

    const HttpRequest req = HttpRequest::get(REMOTE_GPS_TRACKING_CONSENTED_RFIDS_PATH);
    const ApiResponse resp = api.makeRequest(req, true);

    if (!resp.valid)
    {
        fileLog.warningln("Request failed");
        return false;
    }

    fileLog.infoln("Response code: " + String(resp.responseCode));

    if (resp.responseCode != 200)
    {
        fileLog.errorln("Unexpected response code " + String(resp.responseCode));
        return false;
    }

    const uint32_t bytesDownloaded = api.fetch(resp, file);

    file.close();

    if (bytesDownloaded != resp.bodyLength)
    {
        fileLog.errorln(
            "Downloaded size (" + String(bytesDownloaded) + " B) does not match content size (" +
            String(resp.bodyLength) + " B). GPS UIDs not updated");
        LittleFSHelper::remove(TMP_RFID_FILE_PATH);
        return false;
    }

    fileLog.infoln("Successfully downloaded file");

    const bool moveSuccess = LittleFSHelper::move(TMP_RFID_FILE_PATH, GPS_TRACKING_CONSENTED_RFIDS_FILE_PATH, true);
    fileLog.logInfoOrErrorln(moveSuccess, "GPS RFID UIDs updated successfully", "GPS RFID UIDs not updated");
    return moveSuccess;
}


bool RFIDs::RFIDConsentsToGPSTrackingTest(const uint32_t rfid)
{
    File file = LittleFS.open(GPS_TRACKING_CONSENTED_RFIDS_FILE_PATH, FILE_READ);

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
