// Modem.cpp

#include "Modem.h"
#include "Config.h"
#include "Globals.h"
#include <HelperUtils.h>

#include "Backend.h"
#include "LocalConfig.h"
#include "StorageManager.h"
#include "WatchdogHandler.h"

// DownloadStream constructor implementation
DownloadStream::DownloadStream(const String& remotePath, Client& gsmClient, const String& server, const uint16_t port,
                               const String& username, const String& password, const String& cacheChecksum)
    : HttpClient(gsmClient, server, port), isValid(false)
{
    fileLog.infoln("Opening stream to " + remotePath);

    beginRequest();
    const int err = get(remotePath);

    if (err != 0)
    {
        fileLog.errorln("Error " + String(err) + ". Stream open canceled");
        return;
    }

    if (!cacheChecksum.isEmpty())
        sendHeader("if-none-match", cacheChecksum);

    if (!username.isEmpty() && !password.isEmpty())
        sendBasicAuth(username, password);

    endRequest();

    responseCode = HttpClient::responseStatusCode();
    skipResponseHeaders();

    fileLog.infoln("Response status: " + String(responseCode));

    if (responseCode <= 0)
    {
        fileLog.errorln("Unexpected status (see above). Stream open canceled");
        return;
    }

    if (responseCode == 304)
    {
        fileLog.infoln("Not modified. Nothing to download");
        isValid = true;
        return;
    }

    const size_t totalLen = contentLength();
    fileLog.infoln("Content length: " + String(totalLen) + " B");

    const uint32_t downloadTime = totalLen / modem.getEstimatedDownloadSpeed();

    fileLog.debugln("Download will take approximately " + String(downloadTime) + "s");

    if (modem.increaseWatchdogTimeoutForFileDownload(totalLen) != ESP_OK)
    {
        fileLog.errorln("Failed to increase TWDT timeout. Stream open canceled");
        return;
    }

    isValid = true;
}

DownloadStream::DownloadStream(const String& remotePath, const String& cacheChecksum) : DownloadStream(
    remotePath, modem.gsmClient, config.server, config.serverPort, modemIMEI, config.serverPassword, cacheChecksum)
{
}

void Modem::powerOn()
{
    pinMode(PWR_PIN, OUTPUT);
    pinMode(POWER_PIN, OUTPUT);

    digitalWrite(POWER_PIN, HIGH);

    digitalWrite(PWR_PIN, LOW);
    delay(1000);
    digitalWrite(PWR_PIN, HIGH);
}

void Modem::powerOff()
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    delay(1500);
    digitalWrite(PWR_PIN, HIGH);
}

bool Modem::enableGPS()
{
    gsmModem.sendAT("+CGPIO=0,48,1,1");

    if (gsmModem.waitResponse(10000L) != 1)
    {
        fileLog.errorln("Set GPS Power HIGH Failed");
    }

    fileLog.infoln("Enabling GPS... ");

    const bool success = gsmModem.enableGPS();

    fileLog.logInfoOrErrorln(success, "Enabled GPS", "Failed to enable GPS");

    return success;
}


bool Modem::disableGPS()
{
    gsmModem.sendAT("+CGPIO=0,48,1,0");

    if (gsmModem.waitResponse(10000L) != 1)
    {
        fileLog.errorln("Set GPS Power LOW Failed");
    }

    fileLog.infoln("Disabling GPS...");

    const bool success = gsmModem.disableGPS();

    fileLog.logInfoOrErrorln(success, "Disabled GPS", "Failed to disable GPS");

    return success;
}

bool Modem::init(const uint8_t retries)
{
    for (uint8_t attempt = 0; attempt <= retries; ++attempt)
    {
        powerOff();

        if (attempt > 0) gsmClient.stop();

        fileLog.infoln("Initializing modem...");
        powerOn();
        SERIAL_AT.begin(serialBaud, SERIAL_8N1, rxPin, txPin);

        while (!gsmModem.testAT())
            delay(10);

        fileLog.infoln("Modem connected to serial");

        const bool modemInitSuccess = gsmModem.init(config.simPin.c_str());
        fileLog.logInfoOrWarningln(modemInitSuccess, "Modem initialized successfully",
                                   "There was an error while initializing the modem");

        fileLog.infoln("Modem info: " + gsmModem.getModemInfo());

        const SimStatus simStatus = gsmModem.getSimStatus();
        const String msg = "Sim status: " + HelperUtils::simStatusToString(simStatus);
        fileLog.logInfoOrWarningln(simStatus == SIM_READY, msg, msg);

        switch (simStatus)
        {
        case SIM_READY: break;
        case SIM_LOCKED:
            {
                const bool unlockSuccess = gsmModem.simUnlock(config.simPin.c_str());
                fileLog.logInfoOrWarningln(unlockSuccess,
                                           "Sim unlocked successfully", "Attempt no. " + String(attempt + 1) + " of " +
                                           String(retries + 1) + " failed because the SIM is not ready. Retrying...");
                if (unlockSuccess) break;

                continue;
            }
        case SIM_ERROR:
            fileLog.errorln(
                "Attempt no. " + String(attempt + 1) + " of " + String(retries + 1) +
                " failed because the SIM card has an unknown status. Retrying...");
            continue;
        case SIM_ANTITHEFT_LOCKED:
            {
                fileLog.errorln("The SIM card is antitheft locked");
                return false;
            }
        }

        fileLog.infoln("Connecting GPRS...");
        const bool gprsSuccess = gsmModem.gprsConnect(config.apn.c_str(), config.gprsUser.c_str(),
                                                      config.gprsPassword.c_str());
        fileLog.logInfoOrWarningln(gprsSuccess, "GPRS connected successfully", "Failed to connect GPRS");

        if (!gsmModem.isGprsConnected())
        {
            fileLog.warningln(
                "Attempt no. " + String(attempt + 1) + " of " + String(retries + 1) +
                " failed because the GPRS failed to connect. Retrying...");
            continue;
        }

        fileLog.infoln("Waiting for network...");
        const bool networkSuccess = gsmModem.waitForNetwork();
        fileLog.logInfoOrWarningln(networkSuccess, "The modem is now connected to the network",
                                   "The modem did not connect to the network even after waiting");

        if (!gsmModem.isNetworkConnected())
        {
            fileLog.warningln(
                "Attempt no. " + String(attempt + 1) + " of " + String(retries + 1) +
                " failed because the network is not connected. Retrying...");
            continue;
        }

        fileLog.infoln("Syncing time");
        constexpr uint8_t maxNetTimeSyncAttempts = 20;
        uint8_t syncAttempt = 0;
        for (; syncAttempt < maxNetTimeSyncAttempts; ++syncAttempt)
        {
            int year;
            gsmModem.getNetworkTime(&year, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
            if (year < 2070 && year >= 2025) break;
            serialOnlyLog.warningln("Modem fetched nonsensical time (Year " + String(year) + ")");
        }
        timeSynced = syncAttempt < maxNetTimeSyncAttempts;
        HelperUtils::updateSystemTimeWithModem();
        fileLog.logInfoOrWarningln(timeSynced, "Time synced successfully", "Failed to sync time");

        if (!timeSynced)
        {
            fileLog.warningln(
                "Attempt no. " + String(attempt + 1) + " of " + String(retries + 1) +
                " failed because the the modem could not synchronise time. Retrying...");
            continue;
        }

        if (!enableGPS())
        {
            fileLog.warningln(
                "Attempt no. " + String(attempt + 1) + " of " + String(retries + 1) +
                " failed because the gps could not be enabled. Retrying...");
            continue;
        }

        fileLog.infoln("Modem startup completed successfully");
        return true;
    }

    fileLog.criticalln("Failed to start and connect the modem");

    return false;
}

UploadResult Modem::uploadData(const String& endpoint, Stream& stream, const uint32_t streamLen, int* statusCode,
                               String* response, unsigned long* uploadStartMs,
                               unsigned long* uploadEndMs, const bool logToLogFile)
{
    HttpClient uploadHttp{gsmClient, config.server, config.serverPort};

    uploadHttp.beginRequest();

    const int err = uploadHttp.post(endpoint);

    const Log* log = logToLogFile ? &fileLog : &serialOnlyLog;

    if (err != 0)
    {
        log->errorln("Failed to upload file. Error " + String(err));
        return UploadResult::HTTP_REQUEST_ERROR;
    }

    if (increaseWatchdogTimeoutForFileUpload(streamLen) != ESP_OK)
    {
        log->errorln("Failed to increase TWDT timeout. Upload aborted");
        return UploadResult::FAILED_TO_INCREASE_TWDT_TIMEOUT;
    }

    uploadHttp.sendBasicAuth(modemIMEI, config.serverPassword);
    uploadHttp.sendHeader("Content-Type", "application/octet-stream");
    uploadHttp.sendHeader("Content-Length", String(streamLen));
    uploadHttp.beginBody();

    constexpr size_t bufferSize = 512;
    uint8_t buffer[bufferSize];
    size_t totalBytesUploaded = 0, totalBytesRead = 0;
    size_t nextPrint = streamLen / 10;

    log->infoln("Uploading " + String(streamLen) + " B to " + endpoint);

    if (uploadStartMs) *uploadStartMs = millis();

    while (stream.available() && streamLen > totalBytesRead)
    {
        const size_t bytesRead = stream.readBytes(buffer, bufferSize);
        totalBytesRead += bytesRead;

        if (bytesRead <= 0) break;

        size_t bytesSent = uploadHttp.write(buffer, bytesRead);

        uint8_t sendRetry = 0;
        constexpr uint8_t maxSendRetries = 10;

        while (bytesSent == 0 && sendRetry < maxSendRetries)
        {
            bytesSent = uploadHttp.write(buffer, bytesRead);
            ++sendRetry;
        }

        if (sendRetry >= maxSendRetries)
        {
            watchdogHandler.revertTemporaryIncrease();
            fileLog.errorln("Failed to write data to http client");
            return UploadResult::FAILED_TO_SEND_DATA;
        }

        totalBytesUploaded += bytesSent;

        if (totalBytesUploaded >= nextPrint)
        {
            log->debugln("Uploaded " + String(totalBytesUploaded) + " B of " + String(streamLen) + " B");
            nextPrint += streamLen / 10;
        }
    }

    if (uploadEndMs) *uploadEndMs = millis();

    uploadHttp.endRequest();

    log->infoln("Upload complete");

    const int respCode = uploadHttp.responseStatusCode();

    if (statusCode) *statusCode = respCode;
    if (response) *response = uploadHttp.responseBody();

    watchdogHandler.revertTemporaryIncrease();

    fileLog.infoln("Response status code: " + String(respCode));

    if (HelperUtils::isSuccessfulResponse(respCode))
    {
        return UploadResult::SUCCESS;
    }

    return UploadResult::UNEXPECTED_STATUS_CODE;
}

UploadAndRetryResult Modem::uploadDataAndRetry(const String& endpoint, Stream& stream, const uint32_t streamLen,
                                               const uint32_t retries, unsigned long* uploadStartMs,
                                               unsigned long* uploadEndMs,
                                               const bool logToLogFile = true)
{
    UploadResult uploadResult;
    uint32_t attemptNo = 0;

    do
    {
        uploadResult = uploadData(endpoint, stream, streamLen, nullptr, nullptr, uploadStartMs, uploadEndMs,
                                  logToLogFile);

        switch (uploadResult)
        {
        case UploadResult::SUCCESS:
            goto endLoop;
        case UploadResult::HTTP_REQUEST_ERROR:
        case UploadResult::UNEXPECTED_STATUS_CODE:
        case UploadResult::FAILED_TO_SEND_DATA:
        case UploadResult::FAILED_TO_INCREASE_TWDT_TIMEOUT:
            break;
        }

        fileLog.errorln(
            "Attempt No. " + String(attemptNo + 1) + " of " + String(retries + 1) +
            " failed at uploading to " + endpoint);

        ++attemptNo;
    }
    while (attemptNo <= retries);

endLoop:;

    switch (uploadResult)
    {
    case UploadResult::HTTP_REQUEST_ERROR:
        return UploadAndRetryResult::HTTP_REQUEST_ERROR;
    case UploadResult::FAILED_TO_INCREASE_TWDT_TIMEOUT:
        return UploadAndRetryResult::FAILED_TO_INCREASE_TWDT_TIMEOUT;
    case UploadResult::SUCCESS:
        if (attemptNo == 0) return UploadAndRetryResult::SUCCESS;
        return UploadAndRetryResult::SUCCESS_AFTER_RETRYING;
    case UploadResult::UNEXPECTED_STATUS_CODE:
        return UploadAndRetryResult::UNEXPECTED_STATUS_CODE;
    case UploadResult::FAILED_TO_SEND_DATA:
        return UploadAndRetryResult::FAILED_TO_SEND_DATA;
    }

    // ReSharper disable once CppDFAUnreachableCode
    throw std::invalid_argument("Invalid result");
}

UploadFileAndRetryResult Modem::uploadFileAndDelete(const String& endpoint, File& f, const bool deleteIfSuccess,
                                                    const bool deleteAfterRetrying, const uint32_t retries)
{
    if (!f)
    {
        fileLog.errorln("Failed to open file");
        return UploadFileAndRetryResult::FAILED_TO_OPEN_FILE;
    }

    const size_t fileSize = f.size();

    if (fileSize <= 0)
    {
        fileLog.infoln("File is empty. Nothing to upload");
        return UploadFileAndRetryResult::FILE_IS_EMPTY;
    }

    const String filePath = f.path();

    const UploadAndRetryResult uploadResult = uploadDataAndRetry(endpoint, f, fileSize, retries, nullptr, nullptr,
                                                                 filePath != LOG_FILE_PATH);

    f.close();

    if (deleteAfterRetrying || (uploadResult == UploadAndRetryResult::SUCCESS && deleteIfSuccess))
    {
        const bool removeSuccess = StorageManager::remove(filePath);
        fileLog.logInfoOrErrorln(removeSuccess, "Deleted " + filePath + " successfully",
                                 "Failed to delete " + filePath);
    }

    switch (uploadResult)
    {
    case UploadAndRetryResult::HTTP_REQUEST_ERROR:
        return UploadFileAndRetryResult::HTTP_REQUEST_ERROR;
    case UploadAndRetryResult::FAILED_TO_INCREASE_TWDT_TIMEOUT:
        return UploadFileAndRetryResult::FAILED_TO_INCREASE_TWDT_TIMEOUT;
    case UploadAndRetryResult::SUCCESS:
        return UploadFileAndRetryResult::SUCCESS;
    case UploadAndRetryResult::UNEXPECTED_STATUS_CODE:
        return UploadFileAndRetryResult::UNEXPECTED_STATUS_CODE;
    case UploadAndRetryResult::FAILED_TO_SEND_DATA:
        return UploadFileAndRetryResult::FAILED_TO_SEND_DATA;
    case UploadAndRetryResult::SUCCESS_AFTER_RETRYING:
        return UploadFileAndRetryResult::SUCCESS_AFTER_RETRYING;
    }

    // ReSharper disable once CppDFAUnreachableCode
    throw std::invalid_argument("Invalid result");
}

UploadFileAndRetryResult Modem::uploadFileAndDelete(const String& endpoint, const String& filePath,
                                                    const bool deleteIfSuccess, const bool deleteAfterRetrying,
                                                    const uint32_t retries)
{
    if (!LittleFS.exists(filePath))
    {
        fileLog.errorln(filePath + " does not exist");
        return UploadFileAndRetryResult::FILE_DOES_NOT_EXIST;
    }

    File f = LittleFS.open(filePath, FILE_READ);

    return uploadFileAndDelete(endpoint, f, deleteIfSuccess, deleteAfterRetrying, retries);
}


/// returns response status
int Modem::simpleGet(const String& aUrlPath, String* responseBody, const String& username,
                     const String& password)
{
    HttpClient http{gsmClient, config.server, config.serverPort};
    http.beginRequest();
    const int err = http.get(aUrlPath);

    if (err != HTTP_SUCCESS)
    {
        return err;
    }

    if (!username.isEmpty() && !password.isEmpty())
        http.sendBasicAuth(username, password);
    http.endRequest();

    const int responseStatus = http.responseStatusCode();
    http.skipResponseHeaders();

    if (responseBody)
    {
        *responseBody = http.responseBody();
    }

    return responseStatus;
}


/// returns response status
int Modem::simpleGetBin(const String& aUrlPath, uint8_t* responseBody, const size_t size, const String& username,
                        const String& password)
{
    HttpClient http{gsmClient, config.server, config.serverPort};
    http.beginRequest();
    const int err = http.get(aUrlPath);

    if (err != HTTP_SUCCESS)
    {
        return err;
    }

    if (!username.isEmpty() && !password.isEmpty())
        http.sendBasicAuth(username, password);
    http.endRequest();

    const int responseStatus = http.responseStatusCode();
    http.skipResponseHeaders();

    if (responseBody)
    {
        http.read(responseBody, size);
    }

    return responseStatus;
}


DownloadResult Modem::downloadData(const String& remotePath, Stream& f, const String& username,
                                   const String& password, unsigned long* downloadStartMs,
                                   unsigned long* downloadEndMs)
{
    DownloadStream downloadStream{remotePath, gsmClient, config.server, config.serverPort, username, password};

    if (!downloadStream)
    {
        fileLog.errorln("Failed to open stream for download");
        return DownloadResult::FAILED_TO_OPEN_STREAM;
    }

    if (downloadStream.responseStatusCode() != 200)
    {
        fileLog.errorln("Unexpected status code. Download canceled");
        return DownloadResult::UNEXPECTED_STATUS_CODE;
    }

    constexpr size_t bufferSize = 512;
    uint8_t buf[bufferSize];

    const size_t totalLen = downloadStream.contentLength();
    size_t downloaded = 0;
    size_t nextPrint = totalLen / 10;

    fileLog.infoln("Downloading " + String(totalLen) + " B from " + remotePath);

    if (downloadStartMs) *downloadStartMs = millis();

    while (downloadStream.connected() || downloadStream.available())
    {
        while (downloadStream.available())
        {
            const size_t len = downloadStream.readBytes(buf, bufferSize);
            f.write(buf, len);
            downloaded += len;

            if (downloaded >= nextPrint)
            {
                fileLog.debugln("Downloaded " + String(downloaded) + " B of " + String(totalLen) + " B");
                nextPrint += totalLen / 10;
            }
        }
    }

    if (downloadEndMs) *downloadEndMs = millis();

    fileLog.infoln("Download complete");

    return DownloadResult::SUCCESS;
}

void Modem::uploadLog(const bool deleteIfSuccess, const bool deleteAfterRetrying, const uint32_t retries)
{
    File f = LittleFS.open(LOG_FILE_PATH, FILE_READ);
    const size_t fileSize = f.size();
    f.close();
    fileLog.infoln("Uploading log file (" + String(fileSize) + " B)");
    uploadFileAndDelete(LOG_FILE_UPLOAD_ENDPOINT, LOG_FILE_PATH, deleteIfSuccess, deleteAfterRetrying, retries);
}

time_t Modem::getUnixTimestamp()
{
    int year, month, day, hour, minute, second;
    float timezone;
    gsmModem.getNetworkTime(&year, &month, &day, &hour, &minute, &second, &timezone);
    return HelperUtils::dateTimeToUnixTimestamp(year, month, day, hour, minute, second, timezone);
}

esp_err_t Modem::increaseWatchdogTimeoutForFileUpload(const size_t fileSize) const
{
    const uint32_t uploadTime = fileSize / estimatedUploadSpeed; // [s]
    if (uploadTime <= watchdogHandler.getCurrentTimeout() / 10) return ESP_OK; // Doesn't really matter
    return watchdogHandler.increaseTimeoutTemporarily(uploadTime);
}

esp_err_t Modem::increaseWatchdogTimeoutForFileDownload(const size_t fileSize) const
{
    const uint32_t downloadTime = fileSize / estimatedDownloadSpeed; // [s]
    if (downloadTime <= watchdogHandler.getCurrentTimeout() / 10) return ESP_OK; // Doesn't really matter
    return watchdogHandler.increaseTimeoutTemporarily(downloadTime);
}

void Modem::performConnectionSpeedTest()
{
#if !SKIP_ALL_CONNECTION_SPEED_TESTS
    fileLog.infoln("Performing connection speed test");

    RandomStream rs{};

    unsigned long downloadStart, downloadEnd;
    const DownloadResult downloadResult = downloadData(
        DOWNLOAD_TEST_ENDPOINT "?file_size=" + String(CONNECTION_SPEED_TEST_FILE_SIZE), rs, modemIMEI,
        config.serverPassword, &downloadStart, &downloadEnd);

    if (downloadResult == DownloadResult::SUCCESS)
    {
        const float downloadSeconds = static_cast<float>(downloadEnd - downloadStart) / 1000.0f;
        estimatedDownloadSpeed = static_cast<uint32_t>(CONNECTION_SPEED_TEST_FILE_SIZE / downloadSeconds);
        if (estimatedDownloadSpeed == 0) estimatedDownloadSpeed = 1;
        fileLog.infoln("Download test complete. Estimated speed: " + String(estimatedDownloadSpeed) + " B/s");
    }
    else
    {
        fileLog.errorln(
            "Download test failed. Aborting further tests. Defaulting to " + String(estimatedUploadSpeed) + " B/s UP, "
            + String(estimatedDownloadSpeed) + " B/s DOWN");
        return;
    }

    unsigned long uploadStart, uploadEnd;
    const UploadAndRetryResult uploadResult = uploadDataAndRetry(
        UPLOAD_TEST_ENDPOINT, rs, CONNECTION_SPEED_TEST_FILE_SIZE, 3, &uploadStart, &uploadEnd);

    if (uploadResult == UploadAndRetryResult::SUCCESS || uploadResult == UploadAndRetryResult::SUCCESS_AFTER_RETRYING)
    {
        const float uploadSeconds = static_cast<float>(uploadEnd - uploadStart) / 1000.0f;
        estimatedUploadSpeed = static_cast<uint32_t>(CONNECTION_SPEED_TEST_FILE_SIZE / uploadSeconds);
        if (estimatedUploadSpeed == 0) estimatedUploadSpeed = 1;
        fileLog.infoln("Upload test complete. Estimated speed: " + String(estimatedUploadSpeed) + " B/s");
    }
    else
    {
        fileLog.warningln("Upload test failed. Defaulting to " + String(estimatedUploadSpeed) + " B/s");
    }
#endif
}

bool Modem::getGPS(GPS_DATA_t& out)
{
    uint8_t status;
    int year, month, day, hour, minute, second;
    const bool success = gsmModem.getGPS(&status, &out.lat, &out.lon, &out.speed, &out.alt,
                                         reinterpret_cast<int*>(&out.vsat), reinterpret_cast<int*>(&out.usat),
                                         &out.accuracy, &year, &month, &day, &hour, &minute, &second);
    if (!success) return false;

    out.unixTimestamp = HelperUtils::dateTimeToUnixTimestamp(year, month, day, hour, minute, second, 0.0f);

    return true;
}
