#include "LocalConfig.h"

#include <Preferences.h>

bool LocalConfig::save() const
{
    Preferences prefs;
    if (!prefs.begin(prefsName, false)) return false;

    prefs.putString(apnKey, apn);
    prefs.putString(serverKey, server);
    prefs.putUShort(portKey, port);
    prefs.putString(passwordKey, password);

    prefs.end();

    return true;
}

String LocalConfig::toString(const bool withVersion) const
{
    String str;

    if (withVersion)
    {
        str += "Version: " + String(CONFIG_VERSION) + " ";
    }

    str += String(apnKey) + "=" + apn + ";";
    str += String(serverKey) + "=" + server + ";";
    str += String(portKey) + "=" + port + ";";
    str += String(passwordKey) + "=" + password + ";";

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
        prefs.getString(serverKey).c_str(),
        prefs.getUShort(portKey),
        prefs.getString(passwordKey).c_str()
    };

    prefs.end();

    return c;
}
