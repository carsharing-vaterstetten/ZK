#pragma once

#include <optional>
#include <utility>
#include <Arduino.h>

#include "Intern.h"

class LocalConfig
{
    static constexpr auto prefsName = "Config v" CONFIG_VERSION;
public:
    static constexpr auto apnKey = "apn";
    static constexpr auto serverKey = "server";
    static constexpr auto portKey = "port";
    static constexpr auto passwordKey = "password";
    static constexpr const char* allKeys[] = {apnKey, serverKey, portKey, passwordKey};

    // Network
    String apn;

    // Backend
    String server;
    uint16_t port;
    String password;

    LocalConfig(String  apn, String server, const uint16_t port, String password) :
        apn(std::move(apn)), server(std::move(server)), port(port), password(std::move(password))
    {
    }

    bool save() const;

    [[nodiscard]] String toString(bool withVersion = true) const;

    static std::optional<LocalConfig> fromStorage();
};
