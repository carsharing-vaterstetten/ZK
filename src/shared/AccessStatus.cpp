#include "AccessStatus.h"

#include "Globals.h"

void AccessStatus::end()
{
    std::lock_guard lock(mtx);
    prefs.end();
}

void AccessStatus::loadToRAM()
{
    std::lock_guard lock(mtx);

    prefs.begin(storageName, false);

    if (prefs.isKey(loggedInRfidKey))
    {
        const uint32_t loggedInRfid = prefs.getULong(loggedInRfidKey);

        loggedInUID = loggedInRfid;
        consentsToGPSTracking = rfidsManager.RFIDConsentsToGPSTrackingTest(loggedInRfid);
    }
    else
    {
        loggedInUID = std::nullopt;
        consentsToGPSTracking = std::nullopt;
    }
}

bool AccessStatus::isLoggedIn() const
{
    std::lock_guard lock(mtx);
    return loggedInUID.has_value();
}

std::optional<bool> AccessStatus::givesGPSTrackingPermission() const
{
    std::lock_guard lock(mtx);
    return consentsToGPSTracking;
}

std::optional<uint32_t> AccessStatus::getLoggedInUID() const
{
    std::lock_guard lock(mtx);
    return loggedInUID;
}

void AccessStatus::setLoginData(uint32_t uid)
{
    std::lock_guard lock(mtx);
    loggedInUID = uid;
    consentsToGPSTracking = rfidsManager.RFIDConsentsToGPSTrackingTest(uid);
    prefs.putULong(loggedInRfidKey, uid);
}

void AccessStatus::clrLoginData()
{
    std::lock_guard lock(mtx);
    loggedInUID = std::nullopt;
    consentsToGPSTracking = std::nullopt;
    prefs.remove(loggedInRfidKey);
}
