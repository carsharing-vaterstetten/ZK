#pragma once

#include <optional>
#include <utility>
#include <Arduino.h>

class StorableConfig;

class LocalConfig
{
public:
    static constexpr auto apnKey = "apn", serverKey = "server", serverPortKey = "serverPort", serverPasswordKey =
                              "serverPassword", gprsUserKey = "gprsUser", gprsPasswordKey = "gprsPassword", simPinKey =
                              "simPin";
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

    LocalConfig(String apn, String gprsUser, String gprsPassword, String server,
                const uint16_t serverPort, String serverPassword, String simPin) :
        apn(std::move(apn)), gprsUser(std::move(gprsUser)), gprsPassword(std::move(gprsPassword)),
        simPin(std::move(simPin)), server(std::move(server)), serverPassword(std::move(serverPassword)),
        serverPort(serverPort)
    {
    }

    static std::optional<LocalConfig> fromStorage(const char* prefsName);

    [[nodiscard]] String toString() const;
};

class StorableConfig : public LocalConfig
{
    const char* prefsName;

public:
    StorableConfig(const String& apn, const String& gprsUser, const String& gprsPassword, const String& server,
                   const uint16_t serverPort, const String& serverPassword, const String& simPin, const char* prefsName)
        : LocalConfig(apn, gprsUser, gprsPassword, server, serverPort, serverPassword, simPin),
          prefsName(prefsName)
    {
    }

    StorableConfig(const LocalConfig& localConfig, const char* prefsName) : LocalConfig(localConfig),
                                                                            prefsName(prefsName)
    {
    }

    static std::optional<StorableConfig> fromStorage(const char* prefsName)
    {
        const auto localConfig = LocalConfig::fromStorage(prefsName);
        if (!localConfig.has_value()) return std::nullopt;
        return std::optional{StorableConfig{localConfig.value(), prefsName}};
    }

    bool save() const;

    [[nodiscard]] String toString() const;
};

extern LocalConfig config;
