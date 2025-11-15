#include "AccessControl.h"
#include "Config.h"
#include "Globals.h"
#include "HelperUtils.h"
#include "LED.h"
#include "RFIDs.h"

#define STORAGE_VERSION "1"

bool AccessControl::init()
{
    pinMode(OPEN_KEY, OUTPUT);
    pinMode(CLOSE_KEY, OUTPUT);

    const bool storageInitSuccess = persistentStorage.begin("AccCtrl v" STORAGE_VERSION, false);

    if (storageInitSuccess)
    {
        if (persistentStorage.isKey(loggedInRfidKey))
        {
            const uint32_t loggedInRfid = persistentStorage.getULong(loggedInRfidKey);
            cachedLoggedInRfid = loggedInRfid;
            fileLog.infoln("RFID " + String(loggedInRfid, 16) + " is logged in");
            loggedInRfidConsentsToGPSTracking = RFIDs::RFIDConsentsToGPSTrackingTest(loggedInRfid);
            fileLog.infoln(loggedInRfidConsentsToGPSTracking.value()
                               ? "Logged in RFID consents to GPS tracking"
                               : "Logged in RFID does not consent to GPS tracking");
        }
    }
    else
    {
        fileLog.errorln("Failed to initialize persistent storage of access control");
    }

    return storageInitSuccess;
}

void AccessControl::end()
{
    persistentStorage.end();
}

void AccessControl::lockCar()
{
    digitalWrite(CLOSE_KEY, HIGH);
    delay(200);
    digitalWrite(CLOSE_KEY, LOW);
    fileLog.infoln("Car locked");
}


void AccessControl::unlockCar()
{
    digitalWrite(OPEN_KEY, HIGH);
    delay(200);
    digitalWrite(OPEN_KEY, LOW);
    fileLog.infoln("Car unlocked");
}

void AccessControl::login(const uint32_t rfid)
{
    cachedLoggedInRfid = rfid;
    unlockCar();
    loggedInRfidConsentsToGPSTracking = RFIDs::RFIDConsentsToGPSTrackingTest(rfid);
    fileLog.infoln(loggedInRfidConsentsToGPSTracking.value()
                       ? "Logged in RFID consents to GPS tracking"
                       : "Logged in RFID does not consent to GPS tracking");
    persistentStorage.putULong(loggedInRfidKey, rfid);
}

void AccessControl::logout()
{
    cachedLoggedInRfid = std::nullopt;
    loggedInRfidConsentsToGPSTracking = std::nullopt;
    lockCar();
    persistentStorage.remove(loggedInRfidKey);
}

bool AccessControl::toggleLogin(const uint32_t rfid)
{
    if (isLoggedIn())
    {
        logout();
        return false;
    }

    login(rfid);
    return true;
}

bool AccessControl::isLoggedIn() const
{
    return cachedLoggedInRfid.has_value();
}

std::optional<uint32_t> AccessControl::loggedInRfid() const
{
    return cachedLoggedInRfid;
}

bool AccessControl::hasPermissionForGPSTracking() const
{
    if (!loggedInRfidConsentsToGPSTracking.has_value()) return true;
    return loggedInRfidConsentsToGPSTracking.value();
}

AccessControl accessControl;
