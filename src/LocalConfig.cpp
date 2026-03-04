#include "LocalConfig.h"

#include <Preferences.h>

#include "Globals.h"

bool StorableConfig::save() const
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
    str += String(apnKey) + "=" + apn + ";";
    str += String(gprsUserKey) + "=" + gprsUser + ";";
    str += String(gprsPasswordKey) + "=" + gprsPassword + ";";
    str += String(serverKey) + "=" + server + ";";
    str += String(serverPortKey) + "=" + serverPort + ";";
    str += String(serverPasswordKey) + "=" + serverPassword + ";";
    str += String(simPinKey) + "=" + simPin + ";";

    return str;
}


std::optional<LocalConfig> LocalConfig::fromStorage(const char* prefsName)
{
    Preferences prefs;

    if (!prefs.begin(prefsName, true)) return std::nullopt;

    for (const char* key : allKeys)
    {
        if (!prefs.isKey(key))
        {
            fileLog.warningln("Couldnt find required key '" + String(key) + "'");
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
