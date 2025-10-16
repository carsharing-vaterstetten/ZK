// Modem.cpp

#include "Modem.h"
#include "Config.h"
#include "Globals.h"
#include <HelperUtils.h>
#include <ctime>

#include "Backend.h"
#include "StorageManager.h"
#include "WatchdogHandler.h"

#define SERIAL_AT Serial1
#define SERIAL_AT_BAUD 115200U

TinyGsmSim7000* Modem::gsmModem = nullptr;
TinyGsmSim7000::GsmClientSim7000* Modem::gsmClient = nullptr;
bool Modem::isInit = false, Modem::timeSynced = false;
uint32_t Modem::estimatedUploadSpeed = 2500U; // [B/s]
uint32_t Modem::estimatedDownloadSpeed = 5000U; // [B/s]

// DownloadStream constructor implementation
DownloadStream::DownloadStream(const String& remotePath, Client& gsmClient, const String& server, uint16_t port,
                               const String& username, const String& password)
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

    if (!username.isEmpty() && !password.isEmpty())
        sendBasicAuth(username, password);

    endRequest();

    const int status = responseStatusCode();
    skipResponseHeaders();

    fileLog.infoln("Response status: " + String(status));

    if (status != 200)
    {
        fileLog.errorln("Unexpected status (see above). Stream open canceled");
        return;
    }

    const size_t totalLen = contentLength();
    fileLog.infoln("Content length: " + String(totalLen) + " B");

    if (Modem::increaseWatchdogTimeoutForFileDownload(totalLen) != ESP_OK)
    {
        fileLog.errorln("Failed to increase TWDT timeout. Stream open canceled");
        return;
    }

    isValid = true;
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

bool Modem::init(const uint8_t retries)
{
    for (uint8_t attempt = 0; attempt <= retries; ++attempt)
    {
        if (gsmClient != nullptr)
        {
            gsmClient->stop();
            powerOff();
            delete gsmClient;
        }

        delete gsmModem;

        gsmModem = new TinyGsmSim7000{SERIAL_AT};
        gsmClient = new TinyGsmSim7000::GsmClientSim7000{*gsmModem};

        fileLog.infoln("Initializing modem...");
        powerOn();
        SERIAL_AT.begin(SERIAL_AT_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

        while (!gsmModem->testAT())
            delay(10);

        fileLog.infoln("Modem connected to serial");

        const bool modemInitSuccess = gsmModem->init(config.simPin.c_str());
        fileLog.logInfoOrWarningln(modemInitSuccess, "Modem initialized successfully",
                                   "There was an error while initializing the modem");

        fileLog.infoln("Modem info: " + gsmModem->getModemInfo());

        const SimStatus simStatus = gsmModem->getSimStatus();
        const String msg = "Sim status: " + HelperUtils::simStatusToString(simStatus);
        fileLog.logInfoOrWarningln(simStatus == SIM_READY, msg, msg);

        switch (simStatus)
        {
        case SIM_READY: break;
        case SIM_LOCKED:
            {
                const bool unlockSuccess = gsmModem->simUnlock(config.simPin.c_str());
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
        const bool gprsSuccess = gsmModem->gprsConnect(config.apn.c_str(), config.gprsUser.c_str(),
                                                       config.gprsPassword.c_str());
        fileLog.logInfoOrWarningln(gprsSuccess, "GPRS connected successfully", "Failed to connect GPRS");

        if (!gsmModem->isGprsConnected())
        {
            fileLog.warningln(
                "Attempt no. " + String(attempt + 1) + " of " + String(retries + 1) +
                " failed because the GPRS failed to connect. Retrying...");
            continue;
        }

        fileLog.infoln("Waiting for network...");
        const bool networkSuccess = gsmModem->waitForNetwork();
        fileLog.logInfoOrWarningln(networkSuccess, "The modem is now connected to the network",
                                   "The modem did not connect to the network even after waiting");

        if (!gsmModem->isNetworkConnected())
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
            gsmModem->getNetworkTime(&year, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
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

        isInit = true;
        fileLog.infoln("Modem startup completed successfully");
        return true;
    }

    fileLog.criticalln("Failed to start and connect the modem");

    return false;
}

UploadResult Modem::uploadFile(const String& endpoint, File& f, int* statusCode, String* response, const int bufferSize,
                               unsigned long* uploadStartMs, unsigned long* uploadEndMs)
{
    const String filePath = f.path();
    const bool isFileLogFile = filePath == LOG_FILE_PATH;

    f.flush();
    const size_t fileSize = f.size();

    String msg = "Uploading " + filePath + " (" + String(fileSize) + " B) to " + endpoint;

    if (isFileLogFile)
    {
        serialOnlyLog.infoln(msg);
    }
    else
    {
        fileLog.debugln(msg);
    }

    if (fileSize == 0)
    {
        f.close();
        fileLog.warningln("File is empty, not uploading");
        return UploadResult::FILE_IS_EMPTY;
    }

    HttpClient uploadHttp{*gsmClient, config.server, config.serverPort};

    uploadHttp.beginRequest();

    const int err = uploadHttp.post(endpoint);

    if (err != 0)
    {
        f.close();
        fileLog.errorln("Failed to upload file. Error " + String(err));
        return UploadResult::HTTP_REQUEST_ERROR;
    }

    if (increaseWatchdogTimeoutForFileUpload(fileSize) != ESP_OK)
    {
        f.close();
        fileLog.errorln("Failed to increase TWDT timeout. Upload aborted");
        return UploadResult::FAILED_TO_INCREASE_TWDT_TIMEOUT;
    }

    uploadHttp.sendBasicAuth(modemIMEI, config.serverPassword);
    uploadHttp.sendHeader("Content-Type", "application/octet-stream");
    uploadHttp.sendHeader("Content-Length", String(fileSize));
    uploadHttp.beginBody();

    uint8_t buffer[bufferSize];
    size_t totalBytesUploaded = 0;
    size_t nextPrint = fileSize / 10;

    if (uploadStartMs) *uploadStartMs = millis();

    while (f.available())
    {
        const size_t bytesRead = f.read(buffer, bufferSize);

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
            f.close();
            WatchdogHandler::revertTemporaryIncrease();
            fileLog.errorln("Failed to write data to http client");
            return UploadResult::FAILED_TO_SEND_DATA;
        }

        totalBytesUploaded += bytesSent;

        if (totalBytesUploaded >= nextPrint)
        {
            msg = "Uploaded " + String(totalBytesUploaded) + " B of " + String(fileSize) + " B";

            if (isFileLogFile)
            {
                serialOnlyLog.debugln(msg);
            }
            else
            {
                fileLog.debugln(msg);
            }

            nextPrint += fileSize / 10;
        }
    }

    if (uploadEndMs) *uploadEndMs = millis();

    uploadHttp.endRequest();

    const int respCode = uploadHttp.responseStatusCode();

    if (statusCode) *statusCode = respCode;
    if (response) *response = uploadHttp.responseBody();

    f.close();

    WatchdogHandler::revertTemporaryIncrease();

    fileLog.infoln("Response status code: " + String(respCode));

    if (HelperUtils::isSuccessfulResponse(respCode))
    {
        return UploadResult::SUCCESS;
    }

    return UploadResult::UNEXPECTED_STATUS_CODE;
}

UploadAndRetryResult Modem::uploadFileAndDelete(
    const String& endpoint, const String& filePath, const bool deleteIfSuccess,
    const bool deleteAfterRetrying, const uint32_t retries, const int bufferSize,
    unsigned long* uploadStartMs, unsigned long* uploadEndMs)
{
    if (!LittleFS.exists(filePath))
    {
        fileLog.errorln(filePath + " does not exist");
        return UploadAndRetryResult::FILE_DOES_NOT_EXIST;
    }

    UploadResult uploadResult;
    uint32_t attemptNo = 0;

    do
    {
        File f = LittleFS.open(filePath, FILE_READ);

        if (!f)
        {
            fileLog.errorln("Failed to open " + filePath);
            return UploadAndRetryResult::FAILED_TO_OPEN_FILE;
        }

        uploadResult = uploadFile(endpoint, f, nullptr, nullptr, bufferSize, uploadStartMs, uploadEndMs);

        switch (uploadResult)
        {
        case UploadResult::FILE_IS_EMPTY:
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
            " failed at uploading " + filePath + " to " + endpoint);

        ++attemptNo;
    }
    while (attemptNo <= retries);

endLoop:;

    if (deleteAfterRetrying || (uploadResult == UploadResult::SUCCESS && deleteIfSuccess))
    {
        const bool removeSuccess = StorageManager::remove(filePath);
        fileLog.logInfoOrErrorln(removeSuccess, "Deleted " + filePath + " successfully",
                                 "Failed to delete " + filePath);
    }

    switch (uploadResult)
    {
    case UploadResult::FILE_IS_EMPTY:
        return UploadAndRetryResult::FILE_IS_EMPTY;
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


/// returns response status
int Modem::simpleGet(const String& aUrlPath, String* responseBody, const String& username,
                     const String& password)
{
    HttpClient http{*gsmClient, config.server, config.serverPort};
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
    HttpClient http{*gsmClient, config.server, config.serverPort};
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


DownloadResult Modem::downloadFile(const String& remotePath, File& f, const String& username,
                                   const String& password, const int bufferSize, unsigned long* downloadStartMs,
                                   unsigned long* downloadEndMs)
{
    fileLog.infoln("Downloading " + remotePath + " to " + f.path());
    DownloadStream downloadStream{remotePath, *gsmClient, config.server, config.serverPort, username, password};

    if (!downloadStream)
    {
        fileLog.errorln("Failed to open stream for download");
        f.close();
        return DownloadResult::FAILED_TO_OPEN_STREAM;
    }

    uint8_t buf[bufferSize];

    const size_t totalLen = downloadStream.contentLength();
    size_t downloaded = 0;
    size_t nextPrint = totalLen / 10;

    fileLog.infoln("Downloading " + String(totalLen) + " B");

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

    f.close();

    fileLog.infoln("Download complete");

    return DownloadResult::SUCCESS;
}

void Modem::uploadLog(const bool deleteIfSuccess, const bool deleteAfterRetrying, const uint32_t retries)
{
    fileLog.infoln("Uploading log file");
    uploadFileAndDelete(LOG_FILE_UPLOAD_ENDPOINT, LOG_FILE_PATH, deleteIfSuccess, deleteAfterRetrying,
                        retries);
}

time_t Modem::getUnixTimestamp()
{
    int year, month, day, hour, minute, second;
    float timezone;
    gsmModem->getNetworkTime(&year, &month, &day, &hour, &minute, &second, &timezone);
    return HelperUtils::dateTimeToUnixTimestamp(year, month, day, hour, minute, second, timezone);
}

esp_err_t Modem::increaseWatchdogTimeoutForFileUpload(const size_t fileSize)
{
    const uint32_t uploadTime = fileSize / estimatedUploadSpeed; // [s]
    if (uploadTime <= WatchdogHandler::getCurrentTimeout() / 10) return ESP_OK; // Doesn't really matter
    return WatchdogHandler::increaseTimeoutTemporarily(uploadTime);
}

esp_err_t Modem::increaseWatchdogTimeoutForFileDownload(const size_t fileSize)
{
    const uint32_t downloadTime = fileSize / estimatedDownloadSpeed; // [s]
    if (downloadTime <= WatchdogHandler::getCurrentTimeout() / 10) return ESP_OK; // Doesn't really matter
    return WatchdogHandler::increaseTimeoutTemporarily(downloadTime);
}

void Modem::performConnectionSpeedTest()
{
#if !SKIP_ALL_CONNECTION_SPEED_TESTS
    fileLog.infoln("Performing connection speed test");

    // Use LittleFS for simplicity and reliability
    File df = LittleFS.open(CONNECTION_SPEED_TEST_FILE_PATH, FILE_WRITE, true);

    unsigned long downloadStart, downloadEnd;
    const DownloadResult downloadResult = downloadFile(
        DOWNLOAD_TEST_ENDPOINT "?file_size=" + String(CONNECTION_SPEED_TEST_FILE_SIZE), df, modemIMEI,
        config.serverPassword, 512, &downloadStart, &downloadEnd);
    df.close();

    df = LittleFS.open(CONNECTION_SPEED_TEST_FILE_PATH, FILE_READ);
    const size_t fileSize = df.size();
    df.close();

    if (downloadResult == DownloadResult::SUCCESS)
    {
        const float downloadSeconds = static_cast<float>(downloadEnd - downloadStart) / 1000.0f;
        estimatedDownloadSpeed = static_cast<uint32_t>(static_cast<float>(fileSize) / downloadSeconds);
        if (estimatedDownloadSpeed == 0) estimatedDownloadSpeed = 1;
        fileLog.infoln("Download test complete. Estimated speed: " + String(estimatedDownloadSpeed) + " B/s");
    }
    else
    {
        fileLog.errorln(
            "Download test failed. Aborting further tests. Defaulting to " + String(estimatedUploadSpeed) + " B/s UP, "
            + String(estimatedDownloadSpeed) + " B/s DOWN");
        LittleFS.remove(CONNECTION_SPEED_TEST_FILE_PATH);
        return;
    }

    unsigned long uploadStart, uploadEnd;
    const UploadAndRetryResult uploadResult = uploadFileAndDelete(
        UPLOAD_TEST_ENDPOINT, CONNECTION_SPEED_TEST_FILE_PATH, true, true, 3, 512, &uploadStart, &uploadEnd);

    if (uploadResult == UploadAndRetryResult::SUCCESS || uploadResult == UploadAndRetryResult::SUCCESS_AFTER_RETRYING)
    {
        const float uploadSeconds = static_cast<float>(uploadEnd - downloadStart) / 1000.0f;
        estimatedUploadSpeed = static_cast<uint32_t>(static_cast<float>(fileSize) / uploadSeconds);
        if (estimatedUploadSpeed == 0) estimatedUploadSpeed = 1;
        fileLog.infoln("Upload test complete. Estimated speed: " + String(estimatedUploadSpeed) + " B/s");
    }
    else
    {
        fileLog.warningln("Upload test failed. Defaulting to " + String(estimatedUploadSpeed) + " B/s");
    }
#endif
}
