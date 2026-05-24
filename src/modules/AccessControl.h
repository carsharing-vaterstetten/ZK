#pragma once

#include <Preferences.h>

#include "drivers/car_key.h"

class AccessControl
{
public:
    explicit AccessControl(CarKeyDriver& driver) : driver(driver) {}

    bool begin();
    void end();

    // Returns true if the rfid ended up logged in
    bool toggleLogin(uint32_t rfid);
    [[nodiscard]] bool isLoggedIn() const;

    [[nodiscard]] std::optional<uint32_t> loggedInRfid() const;
    [[nodiscard]] bool hasPermissionForGPSTracking() const;

protected:
    CarKeyDriver& driver;

    Preferences persistentStorage{};
    std::optional<uint32_t> cachedLoggedInRfid = std::nullopt;
    std::optional<bool> loggedInRfidConsentsToGPSTracking = std::nullopt;

    static constexpr auto kStorageName = "AccCtrl v1";
    static constexpr auto loggedInRfidKey = "logged in rfid";

    void login(uint32_t rfid);
    void logout();
};
