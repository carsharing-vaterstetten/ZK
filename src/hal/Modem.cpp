#include <algorithm>
#include <esp_system.h>

#include "util/HelperUtils.h"
#include "hal/Modem.h"
#include "net/ApiClient.h"
#include "logging/Loggers.h"
#include "util/Files.h"

Modem::Modem(TinyGsmSim7000& gsmModem, HardwareSerial& hwSerial, const ulong serialBaud,
             const ModemHardware& driver) :
    serialBaud(serialBaud), serial(hwSerial), gsmModem(gsmModem), driver(driver) {}

void Modem::powerOn() const
{
    // The hardware design guide doesnt specify a delay between VBAT and PWRKEY.
    // PWRKEY rises when VBAT (modem internal pull up) is provided, then theoretically the power on sequence can be started immediately
    driver.powerOn();
    logger.infoln("Modem turned on");
}

void Modem::forcePowerCycle() const
{
    logger.warningln("Forcing Modem Power Cycle...");

    driver.cutPower(); // Cut power completely (Mimic Hard Reset)
    delay(2000); // Wait for capacitors to discharge
    driver.providePower(); // Restore power
    delay(500); // Wait for voltage to stabilize

    logger.infoln("Modem power restored");
}


void Modem::wakeup()
{
    logger.debugln("Waking up modem");

    if (modemIsAwake)
    {
        logger.infoln("Modem already awake");
        return;
    }

    driver.wakeup();
    // delay(2000);
    gsmModem.sleepEnable(false);
    modemIsAwake = true;
    logger.debugln("Modem wakeup sent");
}

bool Modem::wakeupAndWait(const uint32_t timeoutMs)
{
    wakeup();
    const bool active = gsmModem.testAT(timeoutMs);
    logger.logInfoOrErrorln(active, "Modem awake and responsive", "Modem not waking up");
    return active;
}

bool Modem::beginSleep()
{
    driver.sleep();
    const bool success = gsmModem.sleepEnable(true);
    logger.logInfoOrWarningln(success, "Modem sent to sleep successfully", "Failed to send modem to sleep");
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
    logger.debugln("Enabling GPS...");

    if (gsmModem.isEnableGPS()) // not necessary?
    {
        gpsIsEnabled = true;
        logger.debugln("GPS already enabled");
        return true;
    }

    const bool success = gsmModem.enableGPS(48, 1); // TODO: set pins in hw config

    logger.logInfoOrCriticalErrorln(success, "Enabled GPS", "Failed to enable GPS");

    if (success)
    {
        gpsIsEnabled = true;
    }

    return success;
}


bool Modem::waitForRDY(uint32_t timeout_ms)
{
     return gsmModem.waitResponse(timeout_ms, "RDY") > 0;
}

bool Modem::connect(const char* simPin, const char* user, const char* password, const char* netApn, const uint retries)
{
    gprsUser = user;
    gprsPassword = password;
    apn = netApn;

    driver.providePower();
    driver.wakeup();

    const esp_reset_reason_t reason = esp_reset_reason();

    // Check if we are coming from a soft restart or a hard power-up
    if (reason == ESP_RST_SW)
    {
        logger.infoln("Modem Hot Start");
        if (beginHot(simPin)) return true;

        logger.warningln("Hot Start failed, attempting Cold Start fallback...");
    }
    else
    {
        logger.infoln("Modem Cold Start");
    }

    return beginCold(simPin, retries);
}

bool Modem::beginHot(const char* simPin)
{
    // Try the target baud rate immediately
    auto [success, detectedBaud] = autoBaud(500);

    if (success)
    {
        logger.infoln("Modem already active at " + String(detectedBaud) + " baud.");
        successfulHotstart = finishInit(simPin, detectedBaud);
    }
    else
        successfulHotstart = false;

    return successfulHotstart;
}

bool Modem::beginCold(const char* simPin, const uint retries)
{
    for (uint attempt = 0; attempt <= retries; ++attempt)
    {
        logger.infoln("Cold Start Attempt " + String(attempt + 1));

        // If the modem was already running, turnOff() ensures a clean start.
        // If it was already off, this pulse might be ignored or act as a toggle.
        // TODO: turnOff();

        // delay(1000);
        powerOn(); // Pulse PWRKEY to boot

        serialLogger.debugln("Waiting for RDY");
        bool a = waitForRDY(10000); // TODO: also needed for hot start?
        serialLogger.debugln("Got " + String(a));
        /*
        serialLogger.debugln("Waiting for CFUN");
        a = gsmModem.waitResponse(10000, "CFUN");
        serialLogger.debugln("Got " + String(a));

        serialLogger.debugln("Waiting for CPIN");
        a = gsmModem.waitResponse(10000, "CPIN");
        serialLogger.debugln("Got " + String(a));

        serialLogger.debugln("Waiting for SMS ready");
        a = gsmModem.waitResponse(10000, "SMS Ready");
        serialLogger.debugln("Got " + String(a));
*/
        // The SIM7000 takes ~4.5s to start its serial interface.
        // We use autoBaud with a 10-second timeout to catch it as it wakes up.
        auto [success, detectedBaud] = autoBaud(10000);

        if (success)
            return finishInit(simPin, detectedBaud);

        logger.errorln("Modem failed to boot. Hard cycling power rail...");

        // Physical recovery: Cut VCC rail to the modem
        forcePowerCycle();
    }
    return false;
}

std::tuple<bool, ulong> Modem::autoBaud(const uint32_t timeoutMs)
{
    logger.debugln("Baud rate scanning...");

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
            logger.debugln("Baud rate " + String(baud) + " SUCCESS");
            return {true, baud};
        }
        logger.debugln("Baud rate " + String(baud) + " failed");
    }

    return {false, 0};
}

bool Modem::finishInit(const char* simPin, const ulong detectedBaud) const
{
    // If the modem is at a different baud than our target, move it.
    if (detectedBaud != serialBaud)
    {
        logger.infoln("Switching modem baud from " + String(detectedBaud) + " to " + String(serialBaud));
        gsmModem.setBaud(serialBaud);
        delay(100);
        serial.updateBaudRate(serialBaud);
        delay(100);
    }

    if (!gsmModem.init(simPin))
    {
        logger.errorln("gsmModem.init() failed");
        return false;
    }

    // Set standard LilyGo/SIM7000 configuration
    gsmModem.setNetworkMode(MODEM_NETWORK_LTE);
    gsmModem.setPreferredMode(MODEM_PREFERRED_CATM);

    logger.infoln("Modem successfully initialized.");
    return true;
}

bool Modem::connectGPRSAndNetwork(const uint retries) const
{
    logger.infoln("Connecting GPRS and network...");

    // TODO: find out best connection order. According to TinyGSM lib: gprs first. My experience: based on previous modem state
    const bool tryGprsFirst = successfulHotstart;

    bool gprsSuccess = gsmModem.isGprsConnected(), networkSuccess = gsmModem.isNetworkConnected();

    if (gprsSuccess && networkSuccess)
    {
        logger.infoln("GPRS and network are already connected");
        return true;
    }

    auto gprs = [this]()-> bool
    {
        logger.infoln("Connecting GPRS...");
        const bool success = gsmModem.gprsConnect(apn, gprsUser, gprsPassword);
        logger.logInfoOrWarningln(success, "GPRS connected successfully", "Failed to connect GPRS");
        return success;
    };

    auto network = [this]()-> bool
    {
        logger.infoln("Connecting network...");
        const bool success = gsmModem.waitForNetwork();
        logger.logInfoOrWarningln(success, "Network connected successfully", "Failed to connect network");
        return success;
    };

    for (uint attempt = 0; attempt <= retries; ++attempt)
    {
        logger.infoln("Attempt " + String(attempt + 1) + " of " + String(retries + 1));

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

bool Modem::ensureNetworkConnection(const uint maxRetries) const
{
    if (!connectGPRSAndNetwork(maxRetries)) return false;

    // Wait for signal. 99 is the modem's "not known or not detectable".
    int16_t signalQuality = gsmModem.getSignalQuality();

    for (uint attempt = 0; attempt < maxRetries && signalQuality == 99; ++attempt)
    {
        logger.debugln("Waiting for signal...");
        delay(2000);
        signalQuality = gsmModem.getSignalQuality();
    }

    if (signalQuality == 99)
    {
        logger.errorln("Could not get signal");
        return false;
    }

    logger.debugln("Got signal");

    return true;
}

bool Modem::disconnectNetwork() const
{
    logger.debugln("Disconnecting GPRS...");

    if (!gsmModem.isGprsConnected())
    {
        logger.infoln("GPRS already disconnected");
        return true;
    }

    const bool success = gsmModem.gprsDisconnect();

    logger.logInfoOrErrorln(success, "GPRS disconnected successfully", "GPRS failed to disconnect");

    return success;
}

ApiResponse Modem::uploadData(ApiClient& api, const char* endpoint, Stream& stream, const size_t streamLen)
{
    const HttpRequest req = HttpRequest::post(endpoint, stream, streamLen, {
                                                  {"Content-Type", "application/octet-stream"}
                                              });
    return api.makeRequest(req, true);
}

UploadAndRetryResult Modem::uploadDataAndRetry(ApiClient& api, const char* endpoint, Stream& stream,
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

        logger.errorln(
            "Attempt No. " + String(attemptNo + 1) + " of " + String(retries + 1) +
            " failed at uploading to " + endpoint);

        ++attemptNo;
    }
    while (attemptNo <= retries);

    return UploadAndRetryResult::FAILED;
}

UploadFileAndRetryResult Modem::uploadFileAndDelete(ApiClient& api, const char* endpoint, File& f,
                                                    const bool deleteIfSuccess,
                                                    const bool deleteAfterRetrying, const uint retries)
{
    if (!f)
    {
        logger.errorln("Failed to open file");
        return UploadFileAndRetryResult::FAILED_TO_OPEN_FILE;
    }

    const size_t fileSize = f.size();
    const String filePath = f.path();

    if (fileSize <= 0)
    {
        logger.infoln(filePath + " is empty. Nothing to upload");
        return UploadFileAndRetryResult::FILE_IS_EMPTY;
    }

    logger.infoln("Uploading " + filePath + " (" + String(fileSize) + " B)");

    const UploadAndRetryResult uploadResult = uploadDataAndRetry(api, endpoint, f, fileSize, retries);

    switch (uploadResult)
    {
    case UploadAndRetryResult::FAILED:
        logger.errorln("Failed to upload " + filePath);
        break;
    case UploadAndRetryResult::SUCCESS_AFTER_RETRYING:
    case UploadAndRetryResult::SUCCESS:
        logger.infoln(filePath + " uploaded successfully");
        break;
    }

    if (deleteAfterRetrying || (uploadResult == UploadAndRetryResult::SUCCESS && deleteIfSuccess))
    {
        f.close();
        const bool removeSuccess = Files::remove(filePath);
        logger.logInfoOrErrorln(removeSuccess, "Deleted " + filePath + " successfully",
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

    return UploadFileAndRetryResult::FAILED;
}

UploadFileAndRetryResult Modem::uploadFileAndDelete(ApiClient& api, const char* endpoint, const char* filePath,
                                                    const bool deleteIfSuccess, const bool deleteAfterRetrying,
                                                    const uint retries)
{
    if (!LittleFS.exists(filePath))
    {
        logger.errorln(String(filePath) + " does not exist");
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
    // GPS_DATA_t is the on-flash/on-wire layout and is byte-packed, so its members
    // are not naturally aligned — vsat sits at offset 16, usat at 17, accuracy at
    // 18. TinyGSM writes whole ints and floats through the pointers it is given,
    // and a 32-bit store to an odd address raises LoadStoreAlignment on the ESP32.
    // Read into properly aligned locals first, then narrow into the packed struct.
    uint8_t status;
    int year, month, day, hour, minute, second;
    int vsat = 0, usat = 0;
    float lat = 0.0f, lon = 0.0f, speed = 0.0f, alt = 0.0f, accuracy = 0.0f;

    const bool success = gsmModem.getGPS(&status, &lat, &lon, &speed, &alt, &vsat, &usat, &accuracy,
                                         &year, &month, &day, &hour, &minute, &second);
    if (!success) return false;

    out.lat = lat;
    out.lon = lon;
    out.speed = speed;
    out.alt = alt;
    out.accuracy = accuracy;
    out.vsat = static_cast<uint8_t>(std::min(vsat, 255));
    out.usat = static_cast<uint8_t>(std::min(usat, 255));
    out.unixTimestamp = HelperUtils::dateTimeToUnixTimestamp(year, month, day, hour, minute, second, 0.0f);

    return true;
}
