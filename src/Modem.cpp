// Modem.cpp

#include "Modem.h"
#include "Config.h"
#include "Globals.h"
#include <HelperUtils.h>
#include <ctime>

#include "Backend.h"
#include "StorageManager.h"

#define SERIAL_AT Serial1
#define SERIAL_AT_BAUD 115200U

TinyGsmSim7000* Modem::gsmModem = nullptr;
TinyGsmSim7000::GsmClientSim7000* Modem::gsmClient = nullptr;
bool Modem::isInit = false;

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

        if (gsmModem->getSimStatus() != SIM_ANTITHEFT_LOCKED)
        {
            const bool simUnlockSuccess = gsmModem->simUnlock(config.GSM_PIN);
            fileLog.logInfoOrWarningln(simUnlockSuccess, "SIM unlocked successfully", "SIM unlocking failed");
        }

        fileLog.infoln("Connecting GPRS...");
        const bool gprsSuccess = gsmModem->gprsConnect(config.apn, config.gprsUser, config.gprsPass);
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

void Modem::end()
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    gsmModem->gprsDisconnect();
    fileLog.infoln("Modem disconnected");
}

UploadResult Modem::uploadFile(const String& endpoint, File& f, int* statusCode, String* response,
                               const String& urlParams, const int bufferSize)
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

    uploadHttp.sendBasicAuth(efuseMacHex, config.password);
    uploadHttp.sendHeader("Content-Type", "application/octet-stream");
    uploadHttp.sendHeader("Content-Length", String(fileSize));
    uploadHttp.beginBody();

    uint8_t buffer[bufferSize];
    size_t totalBytesUploaded = 0;
    size_t nextPrint = fileSize / 10;

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
    *statusCode = uploadHttp.responseStatusCode();
    const String responseBody = uploadHttp.responseBody();

    f.close();
    uploadHttp.stop();

    fileLog.infoln("Response status code: " + String(*statusCode));

    *response = responseBody;

    return UploadResult::SUCCESS;
}

UploadWithSizeCheckResult Modem::uploadFileWithSizeCheck(const String& endpoint, File& f, const String& urlParams,
                                                         const int bufferSize)
{
    const size_t fileSize = f.size();

    String resp;
    int responseStatusCode;

    const UploadResult uploadResult = uploadFile(endpoint, f, &responseStatusCode, &resp, urlParams, bufferSize);

    switch (uploadResult)
    {
    case UploadResult::FILE_IS_EMPTY:
        return UploadWithSizeCheckResult::FILE_IS_EMPTY;
    case UploadResult::HTTP_REQUEST_ERROR:
        return UploadWithSizeCheckResult::HTTP_REQUEST_ERROR;
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

void Modem::uploadFileWithSizeCheckAndDelete(
    const String& endpoint, FS& fileFs, const String& filePath,
    const bool deleteIfSuccess, const bool deleteAfterRetrying,
    const uint32_t retries, const String& urlParams, const int bufferSize)
{
    if (!fileFs.exists(filePath))
    {
        fileLog.errorln(filePath + " does not exist");
        return;
    }

    UploadWithSizeCheckResult uploadResult;
    uint32_t attemptNo = 0;

    do
    {
        File f = fileFs.open(filePath, FILE_READ);

        if (!f)
        {
            fileLog.errorln("Failed to open " + filePath);
            return;
        }

        uploadResult = uploadFileWithSizeCheck(endpoint, f, urlParams, bufferSize);

        switch (uploadResult)
        {
        case UploadWithSizeCheckResult::FILE_IS_EMPTY:
        case UploadWithSizeCheckResult::UNEXPECTED_STATUS_CODE:
        case UploadWithSizeCheckResult::SUCCESS:
            goto endLoop;
        case UploadWithSizeCheckResult::HTTP_REQUEST_ERROR:
        case UploadWithSizeCheckResult::SIZE_CHECK_FAILED:
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
                                   const String& password, const int bufferSize)
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

    f.close();

    fileLog.infoln("Download complete");

    return DownloadResult::SUCCESS;
}

void Modem::uploadLog(const bool deleteIfSuccess, const bool deleteAfterRetrying, const uint32_t retries)
{
    fileLog.infoln("Uploading log");
    uploadFileWithSizeCheckAndDelete(LOG_FILE_UPLOAD_ENDPOINT, *StorageManager::logFileFs, LOG_FILE_PATH,
                                     deleteIfSuccess, deleteAfterRetrying, retries);
}

uint64_t Modem::getUTCTimestamp()
{
    int year, month, day, hour, minute, second;
    float timeZone;
    gsmModem->getNetworkTime(&year, &month, &day, &hour, &minute, &second, &timeZone);

    tm datetime{};

    datetime.tm_year = year - 1900; // Number of years since 1900
    datetime.tm_mon = month - 1; // Number of months since January
    datetime.tm_mday = day;
    datetime.tm_hour = hour;
    datetime.tm_min = minute;
    datetime.tm_sec = second;
    // Daylight Savings must be specified
    // -1 uses the computer's timezone setting
    datetime.tm_isdst = -1;

    return mktime(&datetime);
}
