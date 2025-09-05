// Modem.cpp

#include "Modem.h"
#include "Config.h"
#include "Globals.h"
#include <HelperUtils.h>
#include <ctime>
#include <SD_MMC.h>

#include "Backend.h"
#include "StorageManager.h"

#define SerialAT Serial1

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

bool Modem::init(const bool secondTry)
{
    delete gsmModem;
    delete gsmClient;

    gsmModem = new TinyGsmSim7000{SerialAT};
    gsmClient = new TinyGsmSim7000::GsmClientSim7000{*gsmModem};

    fileLog.infoln("Initializing modem...");

    powerOn();

    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

    while (!gsmModem->testAT())
    {
        delay(10);
    }

    fileLog.infoln("Modem connected");

    const bool modemInitSuccess = gsmModem->init();

    // Most of the time the modem works event if it returns an error
    fileLog.logInfoOrWarningln(modemInitSuccess, "Modem initialized successfully",
                               "There was an error while initializing the modem");

    fileLog.infoln("Modem info: " + gsmModem->getModemInfo());

    if (gsmModem->getSimStatus() != SIM_ANTITHEFT_LOCKED)
    {
        const bool simUnlockSuccess = gsmModem->simUnlock(config.GSM_PIN);
        fileLog.logInfoOrWarningln(simUnlockSuccess, "SIM unlocked successfully", "SIM unlocking failed");
    }

    fileLog.infoln("Connecting Modem...");

    const bool gprsSuccess = gsmModem->gprsConnect(config.apn, config.gprsUser, config.gprsPass);

    fileLog.logInfoOrWarningln(gprsSuccess, "GPRS connected successfully", "Failed to connect GPRS");

    fileLog.infoln("Waiting for network...");

    const bool networkSuccess = gsmModem->waitForNetwork();
    fileLog.logInfoOrWarningln(networkSuccess, "The modem is now connected to the network",
                               "The modem did not connect to the network even after waiting");

    if (gsmModem->isNetworkConnected())
    {
        isInit = true;
        return true;
    }

    if (!secondTry)
    {
        fileLog.infoln("Modem failed to connect on first attempt. Giving it another try.");
        return init(true);
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

int Modem::uploadFile(const String& endpoint, File& f, String* response, const String& urlParams, const int bufferSize)
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
        return 1;
    }

    // Append URL parameters if provided
    String fullEndpoint = endpoint;
    if (urlParams.length() > 0)
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
        return err;
    }

    uploadHttp.sendBasicAuth(config.username, config.password);
    uploadHttp.sendHeader("Content-Type", "application/octet-stream");
    uploadHttp.sendHeader("Content-Length", String(fileSize));
    uploadHttp.beginBody();

    uint8_t buffer[bufferSize];
    size_t totalBytesUploaded = 0;
    size_t nextPrint = fileSize / 10;

    while (f.available())
    {
        const size_t bytesRead = f.read(buffer, sizeof(buffer));
        uploadHttp.write(buffer, bytesRead);
        totalBytesUploaded += bytesRead;

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
    const int status = uploadHttp.responseStatusCode();
    const String responseBody = uploadHttp.responseBody();

    f.close();
    uploadHttp.stop();

    fileLog.infoln("Response status code: " + String(status));

    *response = responseBody;

    return status;
}

bool Modem::uploadFileWithSizeCheck(const String& endpoint, File& f, const String& urlParams, const int bufferSize)
{
    const size_t fileSize = f.size();

    String resp;

    const int respStatus = uploadFile(endpoint, f, &resp, urlParams, bufferSize);

    if (respStatus != 200)
    {
        fileLog.errorln("Unexpected response code (see above)");
        return false;
    }

    const long responseSize = resp.toInt();

    if (responseSize != fileSize)
    {
        fileLog.errorln(
            "File size check failed: local: " + String(fileSize) + " B != uploaded: " + String(responseSize) + " B");
        return false;
    }

    return true;
}

bool Modem::uploadFileWithSizeCheckAndDelete(const String& endpoint, FS& fileFs, File& f, const bool deleteIfSuccess,
                                             const bool deleteAfterRetrying, const uint32_t retries,
                                             const String& urlParams, const int bufferSize)
{
    const String filePath = f.path();

    bool uploadSuccess = uploadFileWithSizeCheck(endpoint, f, urlParams, bufferSize);

    for (uint32_t i = 0; i < retries && !uploadSuccess; ++i)
    {
        fileLog.errorln(
            "Attempt No. " + String(i + 1) + " of " + String(retries) +
            " failed at uploading " + filePath + " to " + endpoint + ". Retrying...");
        uploadSuccess = uploadFileWithSizeCheck(endpoint, f, urlParams, bufferSize);
    }

    if (deleteAfterRetrying || (uploadSuccess && deleteIfSuccess))
    {
        const bool removeSuccess = StorageManager::remove(fileFs, filePath);
        fileLog.logInfoOrErrorln(removeSuccess, "Deleted " + filePath + " successfully",
                                 "Failed to delete " + filePath);
    }

    return uploadSuccess;
}


/// returns response status
int Modem::simpleGet(const String& aUrlPath, String* responseBody)
{
    HttpClient http{*gsmClient, config.server, config.port};
    http.beginRequest();
    const int err = http.get(aUrlPath);

    if (err != HTTP_SUCCESS)
    {
        http.stop();
        return err;
    }

    http.sendBasicAuth(config.username, config.password);
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


bool Modem::downloadFile(const String& remotePath, File& f, const int bufferSize)
{
    fileLog.infoln("Downloading " + remotePath + " to " + f.path());
    String srcPath = f.path();
    HttpClient downloadHttp{*gsmClient, config.server, config.port};

    downloadHttp.beginRequest();
    const int err = downloadHttp.get(remotePath);

    if (err != 0)
    {
        fileLog.errorln("Error " + String(err) + ". Download canceled");
        downloadHttp.stop();
        f.close();
        return false;
    }

    downloadHttp.sendBasicAuth(config.username, config.password);
    downloadHttp.endRequest();

    const int status = downloadHttp.responseStatusCode();
    downloadHttp.skipResponseHeaders();

    fileLog.infoln("Response status: " + String(status));

    if (status != 200)
    {
        fileLog.errorln("Unexpected status (see above). Download canceled");
        downloadHttp.stop();
        f.close();
        return false;
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

    if (downloaded != totalLen)
    {
        fileLog.errorln(
            "Downloaded file size (" + String(downloaded) + " B) != content length (" + String(totalLen) + " B)");
        return false;
    }

    fileLog.infoln("Download complete");

    return true;
}

bool Modem::uploadLog(const bool deleteIfSuccess, const bool deleteAfterRetrying, const uint32_t retries)
{
    fileLog.infoln("Uploading log");
    File file = StorageManager::openLog(FILE_READ);

    if (!file)
    {
        fileLog.errorln("Failed to open log for reading");
        return false;
    }

    return uploadFileWithSizeCheckAndDelete(LOG_FILE_UPLOAD_ENDPOINT, *StorageManager::logFileFs, file, deleteIfSuccess,
                                            deleteAfterRetrying, retries, "efuse_mac=" + String(efuseMac, 16));
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
