// Modem.cpp

#include "Modem.h"
#include "Config.h"
#include "Globals.h"
#include <HelperUtils.h>
#include <ctime>
#include <SD.h>
#include <SPIFFS.h>

#include "Backend.h"
#include "StorageManager.h"
#include "WatchdogHandler.h"

#define SERIAL_AT Serial1
#define SERIAL_AT_BAUD 115200U

TinyGsmSim7000* Modem::gsmModem = nullptr;
TinyGsmSim7000::GsmClientSim7000* Modem::gsmClient = nullptr;
bool Modem::isInit = false;
uint32_t Modem::estimatedUploadSpeed = 5000; // [B/s]
uint32_t Modem::estimatedDownloadSpeed = 5000; // [B/s]

void Modem::powerOn()
{
    pinMode(PWR_PIN, OUTPUT);
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
        delete gsmModem;
        delete gsmClient;

        gsmModem = new TinyGsmSim7000{SERIAL_AT};
        gsmClient = new TinyGsmSim7000::GsmClientSim7000{*gsmModem};

        fileLog.infoln("Initializing modem...");
        powerOn();
        SERIAL_AT.begin(SERIAL_AT_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

        while (!gsmModem->testAT())
            delay(10);

        fileLog.infoln("Modem connected to serial");

        const bool modemInitSuccess = gsmModem->init();
        fileLog.logInfoOrWarningln(modemInitSuccess, "Modem initialized successfully",
                                   "There was an error while initializing the modem");

        fileLog.infoln("Modem info: " + gsmModem->getModemInfo());

        fileLog.infoln("Connecting GPRS...");
        const bool gprsSuccess = gsmModem->gprsConnect(config.apn);
        fileLog.logInfoOrWarningln(gprsSuccess, "GPRS connected successfully", "Failed to connect GPRS");

        fileLog.infoln("Waiting for network...");
        const bool networkSuccess = gsmModem->waitForNetwork();
        fileLog.logInfoOrWarningln(networkSuccess, "The modem is now connected to the network",
                                   "The modem did not connect to the network even after waiting");

        if (gsmModem->isNetworkConnected() && gsmModem->isGprsConnected())
        {
            isInit = true;
            return true;
        }

        fileLog.warningln("Attempt no. " + String(attempt + 1) + " of " + String(retries + 1) + " failed");
        powerOff();
    }

    return false;
}

UploadResult Modem::uploadFile(const String& endpoint, File& f, int* statusCode, String* response,
                               const String& urlParams, const int bufferSize, unsigned long* uploadStartMs,
                               unsigned long* uploadEndMs)
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

    // Append URL parameters if provided
    String fullEndpoint = endpoint;
    if (!urlParams.isEmpty())
    {
        fullEndpoint += "?" + urlParams;
    }

    HttpClient uploadHttp{*gsmClient, config.server, config.port};

    uploadHttp.beginRequest();
    const int err = uploadHttp.post(fullEndpoint);

    if (err != 0)
    {
        f.close();
        uploadHttp.stop();
        fileLog.errorln("Failed to upload file. Error " + String(err));
        return UploadResult::HTTP_REQUEST_ERROR;
    }

    if (increaseWatchdogTimeoutForFileUpload(fileSize) != ESP_OK)
    {
        f.close();
        uploadHttp.stop();
        fileLog.errorln("Failed to increase TWDT timeout. Upload aborted");
        return UploadResult::FAILED_TO_INCREASE_TWDT_TIMEOUT;
    }

    uploadHttp.sendBasicAuth(efuseMacHex, config.password);
    uploadHttp.sendHeader("Content-Type", "application/octet-stream");
    uploadHttp.sendHeader("Content-Length", String(fileSize));
    uploadHttp.beginBody();

    uint8_t buffer[bufferSize];
    size_t totalBytesUploaded = 0;
    size_t nextPrint = fileSize / 10;

    if (uploadStartMs) *uploadStartMs = millis();

    while (f.available() && totalBytesUploaded < fileSize)
    {
        size_t readLength = bufferSize;
        if (totalBytesUploaded + readLength > fileSize)
        {
            readLength = fileSize - totalBytesUploaded;
        }

        if (readLength <= 0) break;

        const size_t bytesRead = f.read(buffer, readLength);

        if (bytesRead <= 0) break;

        const size_t bytesSent = uploadHttp.write(buffer, bytesRead);

        if (bytesSent <= 0) break; // This happens, when the server does not allow the request size.

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

    uploadHttp.endRequest();

    if (uploadEndMs) *uploadEndMs = millis();

    if (statusCode) *statusCode = uploadHttp.responseStatusCode();
    const String responseBody = uploadHttp.responseBody();

    f.close();
    uploadHttp.stop();

    WatchdogHandler::revertTempSet();

    if (statusCode)
        fileLog.infoln("Response status code: " + String(*statusCode));

    if (response) *response = responseBody;

    return UploadResult::SUCCESS;
}

UploadWithSizeCheckResult Modem::uploadFileWithSizeCheck(const String& endpoint, File& f, const String& urlParams,
                                                         const int bufferSize, unsigned long* uploadStartMs,
                                                         unsigned long* uploadEndMs)
{
    const size_t fileSize = f.size();

    String resp;
    int responseStatusCode;

    const UploadResult uploadResult = uploadFile(endpoint, f, &responseStatusCode, &resp, urlParams, bufferSize,
                                                 uploadStartMs, uploadEndMs);

    switch (uploadResult)
    {
    case UploadResult::FILE_IS_EMPTY:
        return UploadWithSizeCheckResult::FILE_IS_EMPTY;
    case UploadResult::HTTP_REQUEST_ERROR:
        return UploadWithSizeCheckResult::HTTP_REQUEST_ERROR;
    case UploadResult::FAILED_TO_INCREASE_TWDT_TIMEOUT:
        return UploadWithSizeCheckResult::FAILED_TO_INCREASE_TWDT_TIMEOUT;
    case UploadResult::SUCCESS:
        break;
    }

    if (responseStatusCode != 200)
    {
        fileLog.errorln("Unexpected status code " + String(responseStatusCode));
        return UploadWithSizeCheckResult::UNEXPECTED_STATUS_CODE;
    }

    const long responseSize = resp.toInt();

    if (responseSize != fileSize)
    {
        fileLog.errorln(
            "File size check failed: local: " + String(fileSize) + " B != uploaded: " + String(responseSize) + " B");
        return UploadWithSizeCheckResult::SIZE_CHECK_FAILED;
    }

    return UploadWithSizeCheckResult::SUCCESS;
}

UploadWithSizeCheckResultAndRetries Modem::uploadFileWithSizeCheckAndDelete(
    const String& endpoint, FS& fileFs, const String& filePath, const bool deleteIfSuccess,
    const bool deleteAfterRetrying, const uint32_t retries, const String& urlParams, const int bufferSize,
    unsigned long* uploadStartMs, unsigned long* uploadEndMs)
{
    if (!fileFs.exists(filePath))
    {
        fileLog.errorln(filePath + " does not exist");
        return UploadWithSizeCheckResultAndRetries::FILE_DOES_NOT_EXIST;
    }

    UploadWithSizeCheckResult uploadResult;
    uint32_t attemptNo = 0;

    do
    {
        File f = fileFs.open(filePath, FILE_READ);

        if (!f)
        {
            fileLog.errorln("Failed to open " + filePath);
            return UploadWithSizeCheckResultAndRetries::FAILED_TO_OPEN_FILE;
        }

        uploadResult = uploadFileWithSizeCheck(endpoint, f, urlParams, bufferSize, uploadStartMs, uploadEndMs);

        switch (uploadResult)
        {
        case UploadWithSizeCheckResult::FILE_IS_EMPTY:
        case UploadWithSizeCheckResult::UNEXPECTED_STATUS_CODE:
        case UploadWithSizeCheckResult::SUCCESS:
            goto endLoop;
        case UploadWithSizeCheckResult::HTTP_REQUEST_ERROR:
        case UploadWithSizeCheckResult::SIZE_CHECK_FAILED:
        case UploadWithSizeCheckResult::FAILED_TO_INCREASE_TWDT_TIMEOUT:
            break;
        }

        fileLog.errorln(
            "Attempt No. " + String(attemptNo + 1) + " of " + String(retries + 1) +
            " failed at uploading " + filePath + " to " + endpoint);

        ++attemptNo;
    }
    while (attemptNo <= retries);

endLoop:;

    if (deleteAfterRetrying || (uploadResult == UploadWithSizeCheckResult::SUCCESS && deleteIfSuccess))
    {
        const bool removeSuccess = StorageManager::remove(fileFs, filePath);
        fileLog.logInfoOrErrorln(removeSuccess, "Deleted " + filePath + " successfully",
                                 "Failed to delete " + filePath);
    }

    switch (uploadResult)
    {
    case UploadWithSizeCheckResult::FILE_IS_EMPTY:
        return UploadWithSizeCheckResultAndRetries::FILE_IS_EMPTY;
    case UploadWithSizeCheckResult::HTTP_REQUEST_ERROR:
        return UploadWithSizeCheckResultAndRetries::HTTP_REQUEST_ERROR;
    case UploadWithSizeCheckResult::FAILED_TO_INCREASE_TWDT_TIMEOUT:
        return UploadWithSizeCheckResultAndRetries::FAILED_TO_INCREASE_TWDT_TIMEOUT;
    case UploadWithSizeCheckResult::SUCCESS:
        if (attemptNo == 0) return UploadWithSizeCheckResultAndRetries::SUCCESS;
        return UploadWithSizeCheckResultAndRetries::SUCCESS_AFTER_RETRYING;
    case UploadWithSizeCheckResult::UNEXPECTED_STATUS_CODE:
        return UploadWithSizeCheckResultAndRetries::UNEXPECTED_STATUS_CODE;
    case UploadWithSizeCheckResult::SIZE_CHECK_FAILED:
        return UploadWithSizeCheckResultAndRetries::SIZE_CHECK_FAILED;
    }

    // ReSharper disable once CppDFAUnreachableCode
    throw std::invalid_argument("Invalid result");
}


/// returns response status
int Modem::simpleGet(const String& aUrlPath, String* responseBody, const String& username,
                     const String& password)
{
    HttpClient http{*gsmClient, config.server, config.port};
    http.beginRequest();
    const int err = http.get(aUrlPath);

    if (err != HTTP_SUCCESS)
    {
        http.stop();
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

    http.stop();

    return responseStatus;
}


DownloadResult Modem::downloadFile(const String& remotePath, File& f, const String& username,
                                   const String& password, const int bufferSize, unsigned long* downloadStartMs,
                                   unsigned long* downloadEndMs)
{
    fileLog.infoln("Downloading " + remotePath + " to " + f.path());
    HttpClient downloadHttp{*gsmClient, config.server, config.port};

    downloadHttp.beginRequest();
    const int err = downloadHttp.get(remotePath);

    if (err != 0)
    {
        fileLog.errorln("Error " + String(err) + ". Download canceled");
        downloadHttp.stop();
        f.close();
        return DownloadResult::HTTP_REQUEST_ERROR;
    }

    if (!username.isEmpty() && !password.isEmpty())
        downloadHttp.sendBasicAuth(username, password);

    downloadHttp.endRequest();

    const int status = downloadHttp.responseStatusCode();
    downloadHttp.skipResponseHeaders();

    fileLog.infoln("Response status: " + String(status));

    if (status != 200)
    {
        fileLog.errorln("Unexpected status (see above). Download canceled");
        downloadHttp.stop();
        f.close();
        return DownloadResult::UNEXPECTED_STATUS_CODE;
    }

    uint8_t buf[bufferSize];

    const size_t totalLen = downloadHttp.contentLength();
    size_t downloaded = 0;
    size_t nextPrint = totalLen / 10;

    fileLog.infoln("Downloading " + String(totalLen) + " B");

    if (increaseWatchdogTimeoutForFileDownload(totalLen) != ESP_OK)
    {
        downloadHttp.stop();
        f.close();
        fileLog.errorln("Failed to increase TWDT timeout. Download aborted");
        return DownloadResult::FAILED_TO_INCREASE_TWDT_TIMEOUT;
    }

    if (downloadStartMs) *downloadStartMs = millis();

    while (downloadHttp.connected() || downloadHttp.available())
    {
        while (downloadHttp.available())
        {
            const size_t len = downloadHttp.readBytes(buf, bufferSize);
            f.write(buf, len);
            downloaded += len;

            if (downloaded >= nextPrint)
            {
                fileLog.debugln("Downloaded " + String(downloaded) + " B of " + String(totalLen) + " B");
                nextPrint += totalLen / 10;
            }
        }
    }

    downloadHttp.stop();

    if (downloadEndMs) *downloadEndMs = millis();

    f.close();

    WatchdogHandler::revertTempSet();

    fileLog.infoln("Download complete");

    return DownloadResult::SUCCESS;
}

void Modem::uploadFileFromAllFileSystem(const String& filePath, const String& endpoint, const bool deleteIfSuccess,
                                        const bool deleteAfterRetrying, const uint32_t retries)
{
    if (SPIFFS.exists(filePath))
    {
        fileLog.infoln("Uploading " + filePath + " from SPIFFS");
        uploadFileWithSizeCheckAndDelete(endpoint, SPIFFS, filePath, deleteIfSuccess,
                                         deleteAfterRetrying, retries, "filesystem=SPIFFS");
    }

    if (StorageManager::isSDCardConnected() && SD.exists(filePath))
    {
        fileLog.infoln("Uploading " + filePath + " from SD-card");
        uploadFileWithSizeCheckAndDelete(endpoint, SD, filePath, deleteIfSuccess,
                                         deleteAfterRetrying, retries, "filesystem=SD-card");
    }
}

void Modem::uploadLogsFromAllFileSystems(const bool deleteIfSuccess, const bool deleteAfterRetrying,
                                         const uint32_t retries)
{
    fileLog.infoln("Uploading log file(s)");
    uploadFileFromAllFileSystem(LOG_FILE_PATH, LOG_FILE_UPLOAD_ENDPOINT, deleteIfSuccess, deleteAfterRetrying, retries);
}

uint64_t Modem::getUnixTimestamp()
{
    int year, month, day, hour, minute, second;
    gsmModem->getNetworkTime(&year, &month, &day, &hour, &minute, &second, nullptr);
    return HelperUtils::dateTimeToUnixTimestamp(year, month, day, hour, minute, second);
}

esp_err_t Modem::increaseWatchdogTimeoutForFileUpload(const size_t fileSize)
{
    const uint32_t uploadTime = fileSize / estimatedUploadSpeed; // [s]
    if (uploadTime <= HW_WATCHDOG_DEFAULT_TIMEOUT / 5) return ESP_OK; // Doesnt really matter
    const uint32_t newWatchdogTime = uploadTime + HW_WATCHDOG_DEFAULT_TIMEOUT; // [s]
    return WatchdogHandler::setTempTimeout(newWatchdogTime);
}

esp_err_t Modem::increaseWatchdogTimeoutForFileDownload(const size_t fileSize)
{
    const uint32_t downloadTime = fileSize / estimatedDownloadSpeed; // [s]
    if (downloadTime <= HW_WATCHDOG_DEFAULT_TIMEOUT / 5) return ESP_OK; // Doesnt really matter
    const uint32_t newWatchdogTime = downloadTime + HW_WATCHDOG_DEFAULT_TIMEOUT; // [s]
    return WatchdogHandler::setTempTimeout(newWatchdogTime);
}

void Modem::performConnectionSpeedTest()
{
#if !SKIP_ALL_CONNECTION_SPEED_TESTS
    fileLog.infoln("Performing connection speed test");

    // Use SPIFFS for simplicity and reliability
    File df = SPIFFS.open(CONNECTION_SPEED_TEST_FILE_PATH, FILE_WRITE, true);

    unsigned long downloadStart, downloadEnd;
    const DownloadResult downloadResult = downloadFile(
        REMOTE_SPEED_TEST_FILE, df, "", "", 512, &downloadStart, &downloadEnd);
    df.close();

    df = SPIFFS.open(CONNECTION_SPEED_TEST_FILE_PATH, FILE_READ);
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
        SPIFFS.remove(CONNECTION_SPEED_TEST_FILE_PATH);
        return;
    }

    unsigned long uploadStart, uploadEnd;
    const UploadWithSizeCheckResultAndRetries uploadResult = uploadFileWithSizeCheckAndDelete(
        "/v1/upload-speed-test", SPIFFS, CONNECTION_SPEED_TEST_FILE_PATH, true, true, 3, "",
        512, &uploadStart, &uploadEnd);

    if (uploadResult == UploadWithSizeCheckResultAndRetries::SUCCESS || uploadResult ==
        UploadWithSizeCheckResultAndRetries::SUCCESS_AFTER_RETRYING)
    {
        const float uploadSeconds = static_cast<float>(uploadEnd - downloadStart) / 1000.0f;
        estimatedUploadSpeed = static_cast<uint32_t>(static_cast<float>(fileSize) / uploadSeconds);
        if (estimatedUploadSpeed == 0) estimatedUploadSpeed = 1;
        fileLog.infoln("Upload test complete. Estimated speed: " + String(estimatedUploadSpeed) + " B/s");
    }
    else
    {
        fileLog.warningln("Upload test failed. Defaulting to " + String(estimatedUploadSpeed));
    }
#endif
}
