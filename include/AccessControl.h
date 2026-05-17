#pragma once

#include <Preferences.h>

class AccessControl
{
    Preferences persistentStorage{};
    static constexpr auto loggedInRfidKey = "logged in rfid";
    std::optional<uint32_t> cachedLoggedInRfid = std::nullopt;
    std::optional<bool> loggedInRfidConsentsToGPSTracking = std::nullopt;

    uint8_t keyOpenPin, keyClosePin, keyPowerPin;
    const char* storageName;

    void unlockCar() const;
    void lockCar() const;
    void turnOnKey() const;
    void turnOffKey() const;

public:
    AccessControl(const uint8_t keyOpenPin, const uint8_t keyClosePin, const uint8_t keyPowerPin,
                  const char* storageNameIdentifier) :
        keyOpenPin(keyOpenPin), keyClosePin(keyClosePin), keyPowerPin(keyPowerPin),
        storageName(storageNameIdentifier) {}

    bool begin();
    void end();
    void login(uint32_t rfid);
    void logout();
    bool toggleLogin(uint32_t rfid);
    [[nodiscard]] bool isLoggedIn() const;
    [[nodiscard]] std::optional<uint32_t> loggedInRfid() const;
    [[nodiscard]] bool hasPermissionForGPSTracking() const;
};
