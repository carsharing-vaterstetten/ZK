#pragma once

#include <optional>
#include <utility>
#include <Arduino.h>

#include "Intern.h"

class LocalConfig
{
    static constexpr auto prefsName = "Config v" CONFIG_VERSION;

public:
    static constexpr auto apnKey = "apn", serverKey = "server", serverPortKey = "serverPort", serverPasswordKey =
                              "serverPassword", gprsUserKey = "gprsUser", gprsPasswordKey = "gprsPassword", simPinKey = "simPin";
    static constexpr const char* allKeys[] = {
        apnKey, serverKey, serverPortKey, serverPasswordKey, gprsUserKey, gprsPasswordKey, simPinKey
    };

    // Network
    String apn, gprsUser, gprsPassword;

    // SIM
    String simPin;

    // Backend
    String server, serverPassword;
    uint16_t serverPort;

    LocalConfig(String apn, String gprsUser, String gprsPassword, String server, const uint16_t serverPort,
                String serverPassword, String simPin) :
        apn(std::move(apn)), gprsUser(std::move(gprsUser)), gprsPassword(std::move(gprsPassword)),
        simPin(std::move(simPin)), server(std::move(server)), serverPassword(std::move(serverPassword)), serverPort(serverPort)
    {
    }

    bool save() const;

    [[nodiscard]] String toString(bool withVersion = true) const;

    static std::optional<LocalConfig> fromStorage();
};
