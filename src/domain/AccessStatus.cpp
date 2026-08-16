#include "domain/AccessStatus.h"

#include "logging/Loggers.h"

void AccessStatus::begin()
{
    prefs.begin(storageName, false);
}

void AccessStatus::loadToRAM()
{
    if (!prefs.isKey(loggedInRfidKey))
    {
        std::lock_guard lock(mtx);
        loggedInUID = std::nullopt;
        consentsToGPSTracking = std::nullopt;
        return;
    }

    const uint32_t loggedInRfid = prefs.getULong(loggedInRfidKey);

    // Looked up before taking mtx, as in setLoginData.
    const bool consents = rfidsManager.RFIDConsentsToGPSTrackingTest(loggedInRfid);

    std::lock_guard lock(mtx);
    loggedInUID = loggedInRfid;
    consentsToGPSTracking = consents;
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
    // Looked up before taking mtx: doing it inside would nest RFIDs::gpsRamMutex
    // under this one, an ordering nothing else needs and nothing enforces.
    const bool consents = rfidsManager.RFIDConsentsToGPSTrackingTest(uid);

    std::lock_guard lock(mtx);
    loggedInUID = uid;
    consentsToGPSTracking = consents;
    prefs.putULong(loggedInRfidKey, uid);
}

void AccessStatus::clrLoginData()
{
    std::lock_guard lock(mtx);
    loggedInUID = std::nullopt;
    consentsToGPSTracking = std::nullopt;
    prefs.remove(loggedInRfidKey);
}
