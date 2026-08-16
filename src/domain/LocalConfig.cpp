#include "domain/LocalConfig.h"
#include <Preferences.h>
#include "logging/Loggers.h"
#include <Arduino.h>

std::optional<StorableConfig> StorableConfig::fromStorage(const char* prefsName)
{
    const auto localConfig = LocalConfig::fromStorage(prefsName);
    if (!localConfig.has_value()) return std::nullopt;
    return std::optional{StorableConfig{localConfig.value(), prefsName}};
}

bool StorableConfig::save() const
{
    std::lock_guard lock(nvsMutex);

    Preferences prefs;
    if (!prefs.begin(prefsName, false)) return false;

    prefs.putString(apnKey, apn);
    prefs.putString(gprsUserKey, gprsUser);
    prefs.putString(gprsPasswordKey, gprsPassword);
    prefs.putString(serverKey, server);
    prefs.putUShort(serverPortKey, serverPort);
    prefs.putString(serverPasswordKey, serverPassword);
    prefs.putString(simPinKey, simPin);

    prefs.end();
    return true;
}

std::optional<LocalConfig> LocalConfig::fromStorage(const char* prefsName)
{
    // RAII Lock: guards concurrent reads/writes on NVS
    std::lock_guard lock(StorableConfig::nvsMutex);

    Preferences prefs;
    if (!prefs.begin(prefsName, true)) return std::nullopt;

    for (const char* key : allKeys)
    {
        if (!prefs.isKey(key))
        {
            logger.warningln("Couldnt find required key '" + String(key) + "'");
            prefs.end();
            return std::nullopt;
        }
    }

    LocalConfig c{
        prefs.getString(apnKey),
        prefs.getString(gprsUserKey),
        prefs.getString(gprsPasswordKey),
        prefs.getString(serverKey),
        prefs.getUShort(serverPortKey),
        prefs.getString(serverPasswordKey),
        prefs.getString(simPinKey),
    };

    prefs.end();
    return c;
}

String LocalConfig::toString() const
{
    String str;
    str += String(apnKey) + "=" + apn + ";";
    str += String(gprsUserKey) + "=" + gprsUser + ";";
    str += String(gprsPasswordKey) + "=" + gprsPassword + ";";
    str += String(serverKey) + "=" + server + ";";
    str += String(serverPortKey) + "=" + serverPort + ";";
    str += String(serverPasswordKey) + "=" + serverPassword + ";";
    str += String(simPinKey) + "=" + simPin + ";";
    return str;
}

String StorableConfig::toString() const
{
    String str;
    str += prefsName;
    str += ": ";
    str += LocalConfig::toString();
    return str;
}

std::optional<LocalConfig> LocalConfig::parse(const String& inputString)
{
    int start = 0;

    std::optional<String> apn, gprsUser, gprsPassword, server, serverPassword, simPin;
    std::optional<uint16_t> serverPort;

    while (start < inputString.length())
    {
        int end = inputString.indexOf(';', start);
        if (end == -1) end = static_cast<int>(inputString.length());

        String token = inputString.substring(start, end);
        token.trim();

        if (token.isEmpty())
        {
            start = end + 1;
            continue;
        }

        const int eqIndex = token.indexOf('=');
        if (eqIndex == -1)
        {
            serialLogger.warningln("Invalid config token (missing '='): '" + token + "'");
            start = end + 1;
            continue;
        }

        String key = token.substring(0, eqIndex);
        String value = token.substring(eqIndex + 1);

        key.trim();
        value.trim();

        if (value.length() >= 2 && value.startsWith("\"") && value.endsWith("\""))
        {
            value = value.substring(1, value.length() - 1);
        }

        if (key == apnKey)
            apn = value;
        else if (key == gprsUserKey)
            gprsUser = value;
        else if (key == gprsPasswordKey)
            gprsPassword = value;
        else if (key == serverKey)
            server = value;
        else if (key == serverPortKey)
            serverPort = value.toInt();
        else if (key == serverPasswordKey)
            serverPassword = value;
        else if (key == simPinKey)
            simPin = value;
        else
            serialLogger.warningln("Unknown config key: '" + key + "'");

        start = end + 1;
    }

    if (!apn || !server || !serverPort || !serverPassword || !gprsUser || !gprsPassword || !simPin) return std::nullopt;

    return LocalConfig{
        apn.value(), gprsUser.value(), gprsPassword.value(), server.value(), serverPort.value(), serverPassword.value(),
        simPin.value()
    };
}

LocalConfig LocalConfig::promptOverSerial()
{
    const LocalConfig exampleConfig{
        "iot.1nce.net",
        "hans",
        "PWD",
        "example.com",
        80,
        "XXX",
        "1234"
    };
    const String exampleConfigFormat = exampleConfig.toString();

    String inputString = "";

    const ulong oldTimeout = Serial.getTimeout();
    Serial.setTimeout(100000000ULL);

    while (true)
    {
        Serial.println("Please enter config data in this format:");
        Serial.println(exampleConfigFormat);

        while (!Serial.available()) {}

        inputString = Serial.readStringUntil('\n');
        inputString.trim();

        if (inputString.isEmpty())
        {
            Serial.println("Entered config is empty");
            continue;
        }

        Serial.println("Entered config string: " + inputString);

        if (const auto pc = parse(inputString))
        {
            Serial.println("Successfully parsed config: " + pc.value().toString());
            Serial.setTimeout(oldTimeout);
            return pc.value();
        }

        Serial.println("Failed to parse config. Try again");
    }
}
