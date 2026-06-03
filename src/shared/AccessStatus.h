#pragma once
#include <mutex>
#include <optional>
#include <Preferences.h>

#include "RFIDs.h"


class AccessStatus
{
public:
    AccessStatus(const char* storageName, const RFIDs& rfidsManager) : storageName(storageName),
                                                                       rfidsManager(rfidsManager) {}

    void end();

    void loadToRAM();

    bool isLoggedIn() const;
    std::optional<bool> givesGPSTrackingPermission() const;
    std::optional<uint32_t> getLoggedInUID() const;

    void setLoginData(uint32_t uid);
    void clrLoginData();

private:
    const char* storageName;

    static constexpr auto loggedInRfidKey = "logged in rfid";

    Preferences prefs;
    const RFIDs& rfidsManager;

    std::optional<uint32_t> loggedInUID = std::nullopt;
    std::optional<bool> consentsToGPSTracking = std::nullopt;

    mutable std::mutex mtx;
};
