#include "net/ApiClient.h"
#include "util/Digest.h"
#include "domain/RFIDs.h"

#include <mbedtls/md5.h>
#include <ArduinoJson.h>

#include "config/Backend.h"
#include "logging/Loggers.h"
#include "util/Files.h"


RFIDs::RFIDs(const char* filePath, const char* tmpFilePath, const char* gpsFilePath, const char* tmpGpsFilePath) :
    filePath(filePath), tmpFilePath(tmpFilePath), gpsFilePath(gpsFilePath), tmpGpsFilePath(tmpGpsFilePath)
{
    rfids = std::make_shared<const std::vector<uint32_t>>();
    gpsRfids = std::make_shared<const std::vector<uint32_t>>();
}

RFIDs::UidList RFIDs::getUids() const
{
    std::lock_guard lock(ramMutex);
    return rfids; // Returns a lightweight copy of the pointer (thread-safe)
}

RFIDs::UidList RFIDs::getGPSUids() const
{
    std::lock_guard lock(gpsRamMutex);
    return gpsRfids;
}

bool RFIDs::isRegisteredRFID(const uint32_t rfid) const
{
    const auto ids = getUids();
    return std::binary_search(ids->begin(), ids->end(), rfid);
}

bool RFIDs::RFIDConsentsToGPSTrackingTest(const uint32_t rfid) const
{
    const auto ids = getGPSUids();
    return std::binary_search(ids->begin(), ids->end(), rfid);
}

/// Reads a UID file into a fresh sorted vector, then swaps it in. Readers hold a
/// shared_ptr, so one mid-lookup keeps the old vector alive until it is done.
bool RFIDs::loadUidsToRam(const char* path, UidList& target, std::mutex& targetMutex, const char* label)
{
    if (!LittleFS.exists(path))
    {
        logger.errorln("Failed to load " + String(label) + " UIDs to RAM, file does not exist");
        return false;
    }

    File f = LittleFS.open(path, FILE_READ);

    if (!f)
    {
        logger.errorln("Failed to open " + String(path) + " for reading");
        return false;
    }

    const size_t uidsCount = f.size() / sizeof(uint32_t);

    auto staged = std::make_shared<std::vector<uint32_t>>(uidsCount);
    f.read(reinterpret_cast<uint8_t*>(staged->data()), uidsCount * sizeof(uint32_t));
    f.close();

    std::sort(staged->begin(), staged->end()); // binary_search needs this

    // Read off the local vector: touching `target` after the lock is released
    // races with whoever swaps next.
    const size_t bytes = staged->capacity() * sizeof(uint32_t);

    {
        std::lock_guard lock(targetMutex);
        target = std::move(staged);
    }

    logger.infoln("Loaded and sorted " + String(uidsCount) + " " + label + " UIDs (consumes " + bytes + " B)");

    return true;
}

/// Call on startup and whenever the file changes on disk (e.g. after a download).
bool RFIDs::loadFromFileToRam()
{
    return loadUidsToRam(filePath, rfids, ramMutex, "RFID");
}

bool RFIDs::loadFromGpsFileToRam()
{
    return loadUidsToRam(gpsFilePath, gpsRfids, gpsRamMutex, "GPS consent");
}

void RFIDs::generateChecksum(uint8_t* out) const
{
    if (!LittleFS.exists(filePath))
    {
        logger.infoln("Local RFIDs file does not exist");
        constexpr uint8_t empty_md5[16] = {
            0xd4, 0x1d, 0x8c, 0xd9,
            0x8f, 0x00, 0xb2, 0x04,
            0xe9, 0x80, 0x09, 0x98,
            0xec, 0xf8, 0x42, 0x7e
        };
        memcpy(out, empty_md5, 16);
        return;
    }

    File f = LittleFS.open(filePath, FILE_READ);
    Digest::md5File(f, out);
    f.close();
}

void RFIDs::downloadRfidsIfChanged(ApiClient& api)
{
    logger.infoln("Downloading remote RFIDs JSON");

    uint8_t md5Checksum[16];
    generateChecksum(md5Checksum);
    String base64Checksum = Digest::toBase64(md5Checksum, 16);

    const HttpRequest req = HttpRequest::get(REMOTE_RFID_PATH, {{"if-none-match", base64Checksum}});
    const ApiResponse resp = api.makeRequest(req, true);

    if (!resp.valid)
    {
        logger.errorln("Request failed");
        return;
    }

    logger.infoln("Response code: " + String(resp.responseCode));

    if (resp.responseCode == 304)
    {
        logger.infoln("Server and local RFID UIDs are the same");
        return;
    }

    if (resp.responseCode != 200)
    {
        logger.errorln("Unexpected response code. Using old RFID UIDs");
        return;
    }

    // Open temp file for writing
    File file = LittleFS.open(tmpFilePath,FILE_WRITE, true);

    if (!file)
    {
        logger.warningln("Failed to open temp RFIDs file. Download canceled");
        return;
    }

    resp.body.setTimeout(100000); // [ms] = 100s. Necessary for JSON parsing

    // Parse JSON from stream
    logger.infoln("Parsing JSON stream");
    JsonDocument doc;
    const DeserializationError error = deserializeJson(doc, resp.body);

    if (error)
    {
        logger.errorln("JSON parsing failed: " + String(error.c_str()));
        file.close();
        Files::remove(tmpFilePath);
        return;
    }

    const auto rfidsArr = doc.as<JsonArray>();

    logger.infoln("Writing " + String(rfidsArr.size()) + " RFIDs to file");
    for (const JsonVariant rfidVariant : rfidsArr)
    {
        const auto rfid = rfidVariant.as<uint32_t>();
        file.write(reinterpret_cast<const uint8_t*>(&rfid), sizeof(rfid));
    }

    file.close();

    logger.infoln("Successfully downloaded and parsed RFIDs file");

    const bool moveSuccess = Files::move(tmpFilePath, filePath, true);

    if (moveSuccess)
        loadFromFileToRam();

    logger.logInfoOrErrorln(moveSuccess, "RFID UIDs updated successfully", "RFID UIDs not updated");
}

bool RFIDs::downloadGPSTrackingConsentedRFIDs(ApiClient& api)
{
    logger.infoln("Downloading remote RFIDs that consent to GPS tracking file");

    File file = LittleFS.open(tmpGpsFilePath, FILE_WRITE, true);

    if (!file)
    {
        logger.warningln("Failed to open temp RFIDs file. Download canceled");
        return false;
    }

    const HttpRequest req = HttpRequest::get(REMOTE_GPS_TRACKING_CONSENTED_RFIDS_PATH);
    const ApiResponse resp = api.makeRequest(req, true);

    if (!resp.valid)
    {
        logger.warningln("Request failed");
        return false;
    }

    logger.infoln("Response code: " + String(resp.responseCode));

    if (resp.responseCode != 200)
    {
        logger.errorln("Unexpected response code " + String(resp.responseCode));
        return false;
    }

    const uint32_t bytesDownloaded = api.fetch(resp, file);

    file.close();

    if (bytesDownloaded != resp.bodyLength)
    {
        logger.errorln(
            "Downloaded size (" + String(bytesDownloaded) + " B) does not match content size (" +
            String(resp.bodyLength) + " B). GPS UIDs not updated");
        Files::remove(tmpGpsFilePath);
        return false;
    }

    logger.infoln("Successfully downloaded file");

    const bool moveSuccess = Files::move(tmpGpsFilePath, gpsFilePath, true);
    logger.logInfoOrErrorln(moveSuccess, "GPS RFID UIDs updated successfully", "GPS RFID UIDs not updated");

    if (moveSuccess)
        loadFromGpsFileToRam();

    return moveSuccess;
}
