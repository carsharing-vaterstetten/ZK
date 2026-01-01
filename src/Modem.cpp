#include "HelperUtils.h"
#include "Modem.h"
#include "Api.h"
#include "Globals.h"
#include "StorageManager.h"

Modem::Modem(HardwareSerial& hwSerial, const uint32_t serialBaud, const int8_t rxPin, const int8_t txPin) :
    serialBaud(serialBaud),
    rxPin(rxPin), txPin(txPin), gsmModem(hwSerial), serial(hwSerial), gsmClient(gsmModem)
{
}

void Modem::powerOn()
{
    pinMode(BOARD_POWERON_PIN, OUTPUT);
    digitalWrite(BOARD_POWERON_PIN, HIGH);
    fileLog.infoln("Modem power on");
}

void Modem::turnOn()
{
    pinMode(BOARD_PWRKEY_PIN, OUTPUT);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);
    delay(100);
    digitalWrite(BOARD_PWRKEY_PIN, HIGH);
    delay(MODEM_POWERON_PULSE_WIDTH_MS);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);
    fileLog.infoln("Modem turned on");
}

void Modem::turnOff()
{
    digitalWrite(BOARD_PWRKEY_PIN, LOW);
    delay(100);
    pinMode(BOARD_PWRKEY_PIN, OUTPUT);
    digitalWrite(BOARD_PWRKEY_PIN, HIGH);
    delay(MODEM_POWEROFF_PULSE_WIDTH_MS);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);
    fileLog.infoln("Modem turned off");
}

bool Modem::powerOff()
{
    fileLog.debugln("Powering off modem...");
    const bool success = gsmModem.poweroff();
    fileLog.logInfoOrErrorln(success, "Modem powered off successfully", "Failed to power off modem");
    return success;
}

void Modem::wakeup()
{
    fileLog.debugln("Waking up modem");

    if (modemIsAwake)
    {
        fileLog.infoln("Modem already awake");
        return;
    }

    pinMode(MODEM_DTR_PIN, OUTPUT);
    digitalWrite(MODEM_DTR_PIN, LOW);
    // delay(2000);
    gsmModem.sleepEnable(false);
    modemIsAwake = true;
    fileLog.debugln("Modem wakeup sent");
}

void Modem::wakeupAndWait(const uint32_t timeoutMs)
{
    wakeup();
    waitForATResponse(timeoutMs);
    fileLog.infoln("Modem awake and responsive");
}

bool Modem::beginSleep()
{
    pinMode(MODEM_DTR_PIN, OUTPUT);
    digitalWrite(MODEM_DTR_PIN, HIGH);
    const bool success = gsmModem.sleepEnable(true);
    fileLog.logInfoOrWarningln(success, "Modem sent to sleep successfully", "Failed to send modem to sleep");
    if (success)
    {
        modemIsAwake = false;
    }
    return success;
}

/// Only sends the modem to sleep if no functions are needed
/// Returns true if the modem was sent to sleep. Returns false if it cannot sleep or already sleeps.
bool Modem::requestSleep()
{
    if (gpsIsEnabled || !modemIsAwake)
        return false;

    return beginSleep();
}

bool Modem::waitForATResponse(const uint32_t timeoutMs)
{
    const unsigned long startMs = millis();

    while (millis() - startMs < timeoutMs)
    {
        if (gsmModem.testAT(100)) return true;
    }

    return false;
}

bool Modem::enableGPS()
{
    fileLog.debugln("Enabling GPS...");

    if (gsmModem.isEnableGPS())
    {
        gpsIsEnabled = true;
        fileLog.debugln("GPS already enabled");
        return true;
    }

    gsmModem.sendAT("+CGPIO=0,48,1,1");

    if (gsmModem.waitResponse(10000L) != 1)
    {
        fileLog.errorln("Set GPS Power HIGH failed");
    }

    const bool success = gsmModem.enableGPS();

    fileLog.logInfoOrCriticalErrorln(success, "Enabled GPS", "Failed to enable GPS");

    if (success)
    {
        gpsIsEnabled = true;
    }

    return success;
}


bool Modem::disableGPS()
{
    fileLog.debugln("Disabling GPS...");

    if (!gsmModem.isEnableGPS())
    {
        gpsIsEnabled = false;
        fileLog.debugln("GPS already disabled");
        return true;
    }

    gsmModem.sendAT("+CGPIO=0,48,1,0");

    if (gsmModem.waitResponse(10000L) != 1)
    {
        fileLog.errorln("Set GPS Power LOW failed");
    }

    const bool success = gsmModem.disableGPS();

    fileLog.logInfoOrErrorln(success, "Disabled GPS", "Failed to disable GPS");

    if (success)
    {
        gpsIsEnabled = false;
    }

    return success;
}

std::tuple<bool, uint32_t> Modem::autoBaud()
{
    fileLog.debugln("Baud rate scanning...");

    serial.updateBaudRate(serialBaud);
    if (waitForATResponse())
        return {true, serialBaud};

    // Higher values are certainly unstable
    constexpr uint32_t baudRates[] = {9600, 115200, 300, 600, 1200, 2400, 4800, 19200, 38400, 57600, 230400, 921600};

    for (const uint32_t baudRate : baudRates)
    {
        serial.updateBaudRate(baudRate);
        if (!waitForATResponse(1000))
        {
            fileLog.debugln("Baud rate " + String(baudRate) + " failed");
            continue;
        }

        return {true, baudRate};
    }

    return {false, 0};
}

bool Modem::begin(const char* simPin, const char* user, const char* password, const char* netApn, const size_t retries)
{
    fileLog.infoln("Initializing modem...");

    gprsUser = user;
    gprsPassword = password;
    apn = netApn;

    for (size_t attempt = 0; attempt <= retries; ++attempt)
    {
        fileLog.infoln("Attempt " + String(attempt + 1) + " of " + String(retries + 1));

        turnOff();

        if (attempt > 0) gsmClient.stop();

        wakeup();
        powerOn();
        turnOn();

        auto [baudSuccess, autoBaudRate] = autoBaud();

        if (!baudSuccess)
        {
            fileLog.warningln("Attempt failed because the modem did not respond to any baud rate");
            continue;
        }

        fileLog.infoln("Modem connected to serial (" + String(autoBaudRate) + " Hz)");

        const bool modemInitSuccess = gsmModem.init(simPin);
        fileLog.logInfoOrWarningln(modemInitSuccess, "Modem initialized successfully",
                                   "There was an error while initializing the modem");

        if (serialBaud != autoBaudRate)
        {
            const uint32_t newBaud = attempt == 0 ? serialBaud : 115200; // fallback baud rate
            gsmModem.setBaud(newBaud);
            serial.flush();
            serial.updateBaudRate(newBaud);
            fileLog.debugln("Set baud rate to " + String(newBaud));
        }

        gsmModem.setNetworkMode(MODEM_NETWORK_LTE);
        gsmModem.setPreferredMode(MODEM_PREFERRED_CATM);

        fileLog.infoln("Modem initialized");

        return true;
    }

    fileLog.criticalln("Failed to initialize the modem");

    return false;
}

bool Modem::ensureNetworkConnection(const size_t maxRetries)
{
    // yes connecting GPRS first and network later is very important. Otherwise, reconnecting doesn't work!

    fileLog.infoln("Connecting modem to network");

    if (!gsmModem.isGprsConnected())
    {
        fileLog.infoln("Connecting GPRS...");

        bool gprsSuccess = false;

        for (size_t retry = 0; retry <= maxRetries; ++retry)
        {
            gprsSuccess = gsmModem.gprsConnect(apn, gprsUser, gprsPassword);

            String msgPrefix = "Try " + String(retry + 1) + " of " + String(maxRetries + 1) + ": ";
            fileLog.logInfoOrWarningln(gprsSuccess, msgPrefix + "GPRS connected successfully",
                                       msgPrefix + "Failed to connect GPRS");

            if (gprsSuccess) break;
        }

        if (!gprsSuccess) return false;
    }
    else
    {
        fileLog.infoln("GPRS already connected");
    }

    if (!gsmModem.isNetworkConnected())
    {
        fileLog.infoln("Waiting for network...");
        const bool networkSuccess = gsmModem.waitForNetwork();
        fileLog.logInfoOrWarningln(networkSuccess, "The modem is now connected to the network",
                                   "The modem did not connect to the network even after waiting");
        if (!networkSuccess) return false;
    }
    else
    {
        fileLog.infoln("Network already connected");
    }

    // Wait for signal
    int16_t signalQuality = gsmModem.getSignalQuality();
    int signalTry = 0;
    for (; signalTry <= maxRetries && signalQuality == 99; ++signalTry)
    {
        fileLog.debugln("Waiting for signal...");
        delay(2000);
        signalQuality = gsmModem.getSignalQuality();
    }

    if (signalTry >= maxRetries)
    {
        fileLog.errorln("Could not get signal");
        return false;
    }

    fileLog.debugln("Got signal");

    return true;
}

bool Modem::disconnectNetwork()
{
    fileLog.debugln("Disconnecting GPRS...");

    if (!gsmModem.isGprsConnected())
    {
        fileLog.infoln("GPRS already disconnected");
        return true;
    }

    const bool success = gsmModem.gprsDisconnect();

    fileLog.logInfoOrErrorln(success, "GPRS disconnected successfully", "GPRS failed to disconnect");

    return success;
}

ApiResponse Modem::uploadData(const char* endpoint, Stream& stream, const uint32_t streamLen)
{
    const HttpRequest req = HttpRequest::post(endpoint, stream, streamLen, {{"Content-Type", "application/octet-stream"}});
    return api.makeRequest(req);
}

UploadAndRetryResult Modem::uploadDataAndRetry(const char* endpoint, Stream& stream, const size_t streamLen,
                                               const size_t retries)
{
    uint32_t attemptNo = 0;

    do
    {
        ApiResponse resp = uploadData(endpoint, stream, streamLen);

        if (resp.valid && resp.responseCode == 200)
        {
            return attemptNo == 0 ? UploadAndRetryResult::SUCCESS : UploadAndRetryResult::SUCCESS_AFTER_RETRYING;
        }

        fileLog.errorln(
            "Attempt No. " + String(attemptNo + 1) + " of " + String(retries + 1) +
            " failed at uploading to " + endpoint);

        ++attemptNo;
    }
    while (attemptNo <= retries);

    return UploadAndRetryResult::FAILED;
}

UploadFileAndRetryResult Modem::uploadFileAndDelete(const char* endpoint, File& f, const bool deleteIfSuccess,
                                                    const bool deleteAfterRetrying, const size_t retries)
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

    const UploadAndRetryResult uploadResult = uploadDataAndRetry(endpoint, f, fileSize, retries);

    f.close();

    // Log to file
    switch (uploadResult)
    {
    case UploadAndRetryResult::FAILED:
        fileLog.errorln("Failed to upload file");
        break;
    case UploadAndRetryResult::SUCCESS_AFTER_RETRYING:
    case UploadAndRetryResult::SUCCESS:
        fileLog.infoln("File uploaded successfully");
        break;
    }

    if (deleteAfterRetrying || (uploadResult == UploadAndRetryResult::SUCCESS && deleteIfSuccess))
    {
        const bool removeSuccess = StorageManager::remove(filePath);
        fileLog.logInfoOrErrorln(removeSuccess, "Deleted " + filePath + " successfully",
                                 "Failed to delete " + filePath);
    }

    switch (uploadResult)
    {
    case UploadAndRetryResult::FAILED:
        return UploadFileAndRetryResult::FAILED;
    case UploadAndRetryResult::SUCCESS:
        return UploadFileAndRetryResult::SUCCESS;
    case UploadAndRetryResult::SUCCESS_AFTER_RETRYING:
        return UploadFileAndRetryResult::SUCCESS_AFTER_RETRYING;
    }

    // ReSharper disable once CppDFAUnreachableCode
    throw std::invalid_argument("Invalid result");
}

UploadFileAndRetryResult Modem::uploadFileAndDelete(const char* endpoint, const char* filePath,
                                                    const bool deleteIfSuccess, const bool deleteAfterRetrying,
                                                    const size_t retries)
{
    if (!LittleFS.exists(filePath))
    {
        fileLog.errorln(String(filePath) + " does not exist");
        return UploadFileAndRetryResult::FILE_DOES_NOT_EXIST;
    }

    File f = LittleFS.open(filePath, FILE_READ);

    return uploadFileAndDelete(endpoint, f, deleteIfSuccess, deleteAfterRetrying, retries);
}

time_t Modem::getUnixTimestamp()
{
    int year, month, day, hour, minute, second;
    float timezone;
    gsmModem.getNetworkTime(&year, &month, &day, &hour, &minute, &second, &timezone);
    return HelperUtils::dateTimeToUnixTimestamp(year, month, day, hour, minute, second, timezone);
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
