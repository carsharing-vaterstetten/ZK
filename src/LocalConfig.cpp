#include "LocalConfig.h"

#include <Preferences.h>

bool LocalConfig::save() const
{
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

String LocalConfig::toString(const bool withVersion) const
{
    String str;

    if (withVersion)
    {
        str += "Version: " CONFIG_VERSION " ";
    }

    str += String(apnKey) + "=" + apn + ";";
    str += String(gprsUserKey) + "=" + gprsUser + ";";
    str += String(gprsPasswordKey) + "=" + gprsPassword + ";";
    str += String(serverKey) + "=" + server + ";";
    str += String(serverPortKey) + "=" + serverPort + ";";
    str += String(serverPasswordKey) + "=" + serverPassword + ";";
    str += String(simPinKey) + "=" + simPin + ";";

    return str;
}


std::optional<LocalConfig> LocalConfig::fromStorage()
{
    Preferences prefs;

    if (!prefs.begin(prefsName, true)) return std::nullopt;

    for (const char* key : allKeys)
        if (!prefs.isKey(key)) return std::nullopt;

    LocalConfig c{
        prefs.getString(apnKey).c_str(),
        prefs.getString(gprsUserKey).c_str(),
        prefs.getString(gprsPasswordKey).c_str(),
        prefs.getString(serverKey).c_str(),
        prefs.getUShort(serverPortKey),
        prefs.getString(serverPasswordKey).c_str(),
        prefs.getString(simPinKey).c_str()
    };

    prefs.end();

    return c;
}
