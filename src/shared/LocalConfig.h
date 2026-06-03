#pragma once

#include <optional>
#include <mutex>
#include <WString.h>

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

    // Thread-safe: read-only immutable data after construction
    const String apn, gprsUser, gprsPassword;
    const String simPin;
    const String server, serverPassword;
    const uint16_t serverPort;

    LocalConfig(String apn, String gprsUser, String gprsPassword, String server,
                const uint16_t serverPort, String serverPassword, String simPin) :
        apn(std::move(apn)), gprsUser(std::move(gprsUser)), gprsPassword(std::move(gprsPassword)),
        simPin(std::move(simPin)), server(std::move(server)), serverPassword(std::move(serverPassword)),
        serverPort(serverPort) {}

    static std::optional<LocalConfig> fromStorage(const char* prefsName);

    [[nodiscard]] String toString() const;
};

class StorableConfig : public LocalConfig
{
    friend class LocalConfig;

    const char* prefsName;

protected:
    inline static std::mutex nvsMutex; // Static mutex shared across all config instances to protect NVS storage

public:
    StorableConfig(const String& apn, const String& gprsUser, const String& gprsPassword, const String& server,
                   const uint16_t serverPort, const String& serverPassword, const String& simPin, const char* prefsName)
        : LocalConfig(apn, gprsUser, gprsPassword, server, serverPort, serverPassword, simPin),
          prefsName(prefsName) {}

    StorableConfig(const LocalConfig& localConfig, const char* prefsName) : LocalConfig(localConfig),
                                                                            prefsName(prefsName) {}

    static std::optional<StorableConfig> fromStorage(const char* prefsName);

    bool save() const;

    [[nodiscard]] String toString() const;

    // Static getter if other systems ever need to synchronize with storage operations
    static std::mutex& getMutex() { return nvsMutex; }
};
