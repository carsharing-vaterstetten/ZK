#include <esp_system.h>

#include "HelperUtils.h"
#include "Modem.h"
#include "Api.h"
#include "Globals.h"
#include "StorageManager.h"

Modem::Modem(TinyGsmSim7000& gsmModem, HardwareSerial& hwSerial, const ulong serialBaud, const int8_t rxPin,
             const int8_t txPin, const uint8_t boardPowerOnPin, const uint8_t boardPWRKeyPin, const uint8_t modemDTRPin,
             const uint32_t modemPowerOnPulseWidthMs, const uint32_t modemPowerOffPulseWidthMs

) : serialBaud(serialBaud), rxPin(rxPin), txPin(txPin), serial(hwSerial), gsmModem(gsmModem),
    boardPowerOnPin(boardPowerOnPin), boardPWRKeyPin(boardPWRKeyPin), modemDTRPin(modemDTRPin),
    modemPowerOnPulseWidthMs(modemPowerOnPulseWidthMs), modemPowerOffPulseWidthMs(modemPowerOffPulseWidthMs) {}

void Modem::powerOn() const
{
    pinMode(boardPowerOnPin, OUTPUT);
    digitalWrite(boardPowerOnPin, HIGH);
    fileLog.infoln("Modem power on");
}

void Modem::turnOn() const
{
    pinMode(boardPWRKeyPin, OUTPUT);
    digitalWrite(boardPWRKeyPin, LOW);
    delay(100);
    digitalWrite(boardPWRKeyPin, HIGH);
    delay(modemPowerOnPulseWidthMs);
    digitalWrite(boardPWRKeyPin, LOW);
    fileLog.infoln("Modem turned on");
}

void Modem::turnOff() const
{
    digitalWrite(boardPWRKeyPin, LOW);
    delay(100);
    pinMode(boardPWRKeyPin, OUTPUT);
    digitalWrite(boardPWRKeyPin, HIGH);
    delay(modemPowerOffPulseWidthMs);
    digitalWrite(boardPWRKeyPin, LOW);
    fileLog.infoln("Modem turned off");
}

bool Modem::powerOff() const
{
    fileLog.debugln("Powering off modem...");
    const bool success = gsmModem.poweroff();
    fileLog.logInfoOrErrorln(success, "Modem powered off successfully", "Failed to power off modem");
    return success;
}


void Modem::forcePowerCycle() const
{
    fileLog.warningln("Forcing Modem Power Cycle...");

    digitalWrite(boardPowerOnPin, LOW); // Cut power completely (Mimic Hard Reset)
    delay(2000); // Wait for capacitors to discharge
    digitalWrite(boardPowerOnPin, HIGH); // Restore power
    delay(500); // Wait for voltage to stabilize

    fileLog.infoln("Modem power restored");
}

void Modem::wakeup()
{
    fileLog.debugln("Waking up modem");

    if (modemIsAwake)
    {
        fileLog.infoln("Modem already awake");
        return;
    }

    pinMode(modemDTRPin, OUTPUT);
    digitalWrite(modemDTRPin, LOW);
    // delay(2000);
    gsmModem.sleepEnable(false);
    modemIsAwake = true;
    fileLog.debugln("Modem wakeup sent");
}

void Modem::wakeupAndWait(const uint32_t timeoutMs)
{
    wakeup();
    gsmModem.testAT(timeoutMs);
    fileLog.infoln("Modem awake and responsive");
}

bool Modem::beginSleep()
{
    pinMode(modemDTRPin, OUTPUT);
    digitalWrite(modemDTRPin, HIGH);
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
SleepRequestResult Modem::requestSleep()
{
    if (!modemIsAwake)
        return SleepRequestResult::AlreadySleeping;

    if (gpsIsEnabled)
        return SleepRequestResult::FailedBecauseModemIsStillInUse;

    return beginSleep() ? SleepRequestResult::Success : SleepRequestResult::Failed;
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

    if (gsmModem.waitResponse(5000L) != 1)
    {
        fileLog.errorln("Set GPS Power LOW failed");
        return false;
    }

    const bool success = gsmModem.disableGPS();

    fileLog.logInfoOrErrorln(success, "Disabled GPS", "Failed to disable GPS");

    if (success)
    {
        gpsIsEnabled = false;
    }

    return success;
}

bool Modem::begin(const char* simPin, const char* user, const char* password, const char* netApn, const uint retries)
{
    gprsUser = user;
    gprsPassword = password;
    apn = netApn;

    wakeup();

    const esp_reset_reason_t reason = esp_reset_reason();

    // Check if we are coming from a soft restart or a hard power-up
    if (reason == ESP_RST_SW)
    {
        fileLog.infoln("Modem Hot Start");
        if (beginHot(simPin)) return true;

        fileLog.warningln("Hot Start failed, attempting Cold Start fallback...");
    }
    else
    {
        fileLog.infoln("Modem Cold Start");
    }

    return beginCold(simPin, retries);
}

bool Modem::beginHot(const char* simPin)
{
    // Try the target baud rate immediately
    auto [success, detectedBaud] = autoBaud(500);

    if (success)
    {
        fileLog.infoln("Modem already active at " + String(detectedBaud) + " baud.");
        return finishInit(simPin, detectedBaud);
    }

    return false;
}

bool Modem::beginCold(const char* simPin, const uint retries)
{
    for (uint attempt = 0; attempt <= retries; ++attempt)
    {
        fileLog.infoln("Cold Start Attempt " + String(attempt + 1));

        powerOn(); // boardPowerOnPin HIGH

        // If the modem was already running, turnOff() ensures a clean start.
        // If it was already off, this pulse might be ignored or act as a toggle.
        turnOff();
        delay(1000);
        turnOn(); // Pulse PWRKEY to boot

        // The SIM7000 takes ~4.5s to start its serial interface.
        // We use autoBaud with a 10-second timeout to catch it as it wakes up.
        auto [success, detectedBaud] = autoBaud(10000);

        if (success)
            return finishInit(simPin, detectedBaud);

        fileLog.errorln("Modem failed to boot. Hard cycling power rail...");

        // Physical recovery: Cut VCC rail to the modem
        forcePowerCycle();
    }
    return false;
}

std::tuple<bool, ulong> Modem::autoBaud(const uint32_t timeoutMs)
{
    fileLog.debugln("Baud rate scanning...");

    // 1. Try the user-defined serialBaud first
    serial.updateBaudRate(serialBaud);
    if (gsmModem.testAT(timeoutMs))
        return {true, serialBaud};

    // 2. Scan fallback baud rates if the first check failed
    constexpr ulong baudRates[] = {115200, 9600, 19200, 38400, 57600, 230400, 921600};

    for (const ulong baud : baudRates)
    {
        if (baud == serialBaud) continue;

        serial.updateBaudRate(baud);
        delay(20); // Give the UART hardware a moment to stabilize

        if (gsmModem.testAT(500))
        {
            fileLog.debugln("Baud rate " + String(baud) + " SUCCESS");
            return {true, baud};
        }
        fileLog.debugln("Baud rate " + String(baud) + " failed");
    }

    return {false, 0};
}

bool Modem::finishInit(const char* simPin, const ulong detectedBaud) const
{
    // If the modem is at a different baud than our target, move it.
    if (detectedBaud != serialBaud)
    {
        fileLog.infoln("Switching modem baud from " + String(detectedBaud) + " to " + String(serialBaud));
        gsmModem.setBaud(serialBaud);
        delay(100);
        serial.updateBaudRate(serialBaud);
        delay(100);
    }

    if (!gsmModem.init(simPin))
    {
        fileLog.errorln("gsmModem.init() failed");
        return false;
    }

    // Set standard LilyGo/SIM7000 configuration
    gsmModem.setNetworkMode(MODEM_NETWORK_LTE);
    gsmModem.setPreferredMode(MODEM_PREFERRED_CATM);

    fileLog.infoln("Modem successfully initialized.");
    return true;
}

bool Modem::connectGPRSAndNetwork(const bool tryGprsFirst, const uint retries) const
{
    fileLog.infoln("Connecting GPRS and network...");

    bool gprsSuccess = gsmModem.isGprsConnected(), networkSuccess = gsmModem.isNetworkConnected();

    if (gprsSuccess && networkSuccess)
    {
        fileLog.infoln("GPRS and network are already connected");
        return true;
    }

    auto gprs = [this]()-> bool
    {
        fileLog.infoln("Connecting GPRS...");
        const bool success = gsmModem.gprsConnect(apn, gprsUser, gprsPassword);
        fileLog.logInfoOrWarningln(success, "GPRS connected successfully", "Failed to connect GPRS");
        return success;
    };

    auto network = [this]()-> bool
    {
        fileLog.infoln("Connecting network...");
        const bool success = gsmModem.waitForNetwork();
        fileLog.logInfoOrWarningln(success, "Network connected successfully", "Failed to connect network");
        return success;
    };

    for (uint attempt = 0; attempt <= retries; ++attempt)
    {
        fileLog.infoln("Attempt " + String(attempt + 1) + " of " + String(retries + 1));

        if (tryGprsFirst && !gprsSuccess)
            gprsSuccess = gprs();

        if (!networkSuccess)
            networkSuccess = network();

        if (!tryGprsFirst && !gprsSuccess)
            gprsSuccess = gprs();

        if (gprsSuccess && networkSuccess) return true;
    }

    return false;
}

bool Modem::ensureNetworkConnection(const bool tryGprsFirst, const uint maxRetries) const
{
    if (!connectGPRSAndNetwork(tryGprsFirst, maxRetries)) return false;

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

bool Modem::disconnectNetwork() const
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

ApiResponse Modem::uploadData(const ApiClient& api, const char* endpoint, Stream& stream, const size_t streamLen)
{
    const HttpRequest req = HttpRequest::post(endpoint, stream, streamLen, {
                                                  {"Content-Type", "application/octet-stream"}
                                              });
    return api.makeRequest(req, true);
}

UploadAndRetryResult Modem::uploadDataAndRetry(const ApiClient& api, const char* endpoint, Stream& stream,
                                               const size_t streamLen,
                                               const uint retries)
{
    uint attemptNo = 0;

    do
    {
        ApiResponse resp = uploadData(api, endpoint, stream, streamLen);

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

UploadFileAndRetryResult Modem::uploadFileAndDelete(const ApiClient& api, const char* endpoint, File& f,
                                                    const bool deleteIfSuccess,
                                                    const bool deleteAfterRetrying, const uint retries)
{
    if (!f)
    {
        fileLog.errorln("Failed to open file");
        return UploadFileAndRetryResult::FAILED_TO_OPEN_FILE;
    }

    const size_t fileSize = f.size();
    const String filePath = f.path();

    if (fileSize <= 0)
    {
        fileLog.infoln(filePath + " is empty. Nothing to upload");
        return UploadFileAndRetryResult::FILE_IS_EMPTY;
    }

    fileLog.infoln("Uploading " + filePath + " (" + String(fileSize) + " B)");

    const UploadAndRetryResult uploadResult = uploadDataAndRetry(api, endpoint, f, fileSize, retries);

    switch (uploadResult)
    {
    case UploadAndRetryResult::FAILED:
        fileLog.errorln("Failed to upload " + filePath);
        break;
    case UploadAndRetryResult::SUCCESS_AFTER_RETRYING:
    case UploadAndRetryResult::SUCCESS:
        fileLog.infoln(filePath + " uploaded successfully");
        break;
    }

    if (deleteAfterRetrying || (uploadResult == UploadAndRetryResult::SUCCESS && deleteIfSuccess))
    {
        f.close();
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

UploadFileAndRetryResult Modem::uploadFileAndDelete(const ApiClient& api, const char* endpoint, const char* filePath,
                                                    const bool deleteIfSuccess, const bool deleteAfterRetrying,
                                                    const uint retries)
{
    if (!LittleFS.exists(filePath))
    {
        fileLog.errorln(String(filePath) + " does not exist");
        return UploadFileAndRetryResult::FILE_DOES_NOT_EXIST;
    }

    File f = LittleFS.open(filePath, FILE_READ);
    const auto res = uploadFileAndDelete(api, endpoint, f, deleteIfSuccess, deleteAfterRetrying, retries);
    f.close(); // it is possible that file may not have been closed
    return res;
}

time_t Modem::getUnixTimestamp() const
{
    int year, month, day, hour, minute, second;
    float timezone;
    gsmModem.getNetworkTime(&year, &month, &day, &hour, &minute, &second, &timezone);
    return HelperUtils::dateTimeToUnixTimestamp(year, month, day, hour, minute, second, timezone);
}

bool Modem::getGPS(GPS_DATA_t& out) const
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
