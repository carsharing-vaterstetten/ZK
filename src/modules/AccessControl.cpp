#include "AccessControl.h"
#include "Globals.h"
#include "logic/HelperUtils.h"
#include "../logic/RFIDs.h"

bool AccessControl::begin()
{
    driver.begin();

    const bool storageInitSuccess = persistentStorage.begin(kStorageName, false);

    if (storageInitSuccess)
    {
        if (persistentStorage.isKey(loggedInRfidKey))
        {
            const uint32_t loggedInRfid = persistentStorage.getULong(loggedInRfidKey);
            cachedLoggedInRfid = loggedInRfid;
            loggedInRfidConsentsToGPSTracking = RFIDs::RFIDConsentsToGPSTrackingTest(loggedInRfid);

            fileLog.infoln("RFID " + String(loggedInRfid, 16) + " is logged in");
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

void AccessControl::login(const uint32_t rfid)
{
    cachedLoggedInRfid = rfid;
    loggedInRfidConsentsToGPSTracking = RFIDs::RFIDConsentsToGPSTrackingTest(rfid);

    driver.open();

    fileLog.infoln(loggedInRfidConsentsToGPSTracking.value()
                       ? "Logged in RFID consents to GPS tracking"
                       : "Logged in RFID does not consent to GPS tracking");

    persistentStorage.putULong(loggedInRfidKey, rfid);
}

void AccessControl::logout()
{
    driver.close();

    cachedLoggedInRfid = std::nullopt;
    loggedInRfidConsentsToGPSTracking = std::nullopt;
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
