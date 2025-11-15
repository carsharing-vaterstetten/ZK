#pragma once

#include <Arduino.h>
#include <Preferences.h>

class AccessControl
{
    Preferences persistentStorage;
    static constexpr auto loggedInRfidKey = "logged in rfid";
    std::optional<uint32_t> cachedLoggedInRfid;
    std::optional<bool> loggedInRfidConsentsToGPSTracking;

    static void unlockCar();
    static void lockCar();

public:
    bool init();
    void end();
    void login(uint32_t rfid);
    void logout();
    bool toggleLogin(uint32_t rfid);
    [[nodiscard]] bool isLoggedIn() const;
    [[nodiscard]] std::optional<uint32_t> loggedInRfid() const;
    [[nodiscard]] bool hasPermissionForGPSTracking() const;
};

extern AccessControl accessControl;
