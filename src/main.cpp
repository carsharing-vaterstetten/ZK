#include <Arduino.h>
#include <Modem.h>
#include <NFC.h>
#include <SPIFFSUtils.h>
#include <HelperUtils.h>
#include <LED.h>

bool loggedIn = false;
unsigned long targetMillis;
const unsigned long dayMillis = 24 * 60 * 60 * 1000; // Ein Tag in Millisekunden
String MAC_ADDRESS;

Modem modem;
NFC nfc;
LED LED_Strip;
Config config;

void initKeyPins()
{
    pinMode(OPEN_KEY, OUTPUT);
    pinMode(CLOSE_KEY, OUTPUT);
}

// Funktion überprüft, ob die gescannte Karte in der Speicherdatei vorhanden ist.
// Wenn ja, wird je nach Zustand loggedIn auf true oder false gesetzt.
void checkNFCTag()
{
    String readValue = nfc.readTag();
    if (readValue != "")
    {
        Serial.println(readValue);
        if (SPIFFSUtils::isRfidInSPIFFS(readValue))
        {
            loggedIn = !loggedIn;
            if (loggedIn)
            {
                digitalWrite(OPEN_KEY, HIGH);
                delay(200);
                LED_Strip.setStaticColor("green");
                digitalWrite(OPEN_KEY, LOW);
                SPIFFSUtils::addLogEntry("Auto aufgesperrt");
            }
            else
            {
                digitalWrite(CLOSE_KEY, HIGH);
                delay(200);
                LED_Strip.setStaticColor("red");
                digitalWrite(CLOSE_KEY, LOW);
                SPIFFSUtils::addLogEntry("Auto zugesperrt");
            }
            delay(2000);
        }
        else
        {
            SPIFFSUtils::addLogEntry("Unbekannte RFID-Karte gescannt");
            LED_Strip.setStaticColor("red");
            delay(2000);
        }
        LED_Strip.clear();
    }
}

void initTime()
{

    String time = modem.getLocalTime();
    Serial.println("Local Time: " + time);

    // Zeitformat "24/11/03,15:01:03+04" (YY/MM/DD,HH:MM:SS+TZ)
    String hourStr = time.substring(9, 11);
    String minStr = time.substring(12, 14);
    String secStr = time.substring(15, 17);

    int hour = hourStr.toInt();
    int minute = minStr.toInt();
    int second = secStr.toInt();

    // Berechne die Millisekunden seit Mitternacht
    unsigned long currentMillis = (hour * 3600 + minute * 60 + second) * 1000;

    if (currentMillis < targetTimeToRestartESP32)
    {
        targetMillis = targetTimeToRestartESP32 - currentMillis;
    }
    else
    {
        targetMillis = dayMillis - (currentMillis - targetTimeToRestartESP32);
    }

    targetMillis += millis(); // Setze Zielzeit relativ zu millis()
}

void setup()
{
    Serial.begin(UART_BAUD);
    while (!Serial)
    {
    };

    HelperUtils::initEEPROM(config);

    if (!HelperUtils::loadConfigFromEEPROM(config))
    {
        delay(5000);
        Serial.println("Bitte Konfigurationsdaten eingeben:");
        Serial.println("Format: apn=\"\";gprsUser=\"\";gprsPass=\"\";GSM_PIN=\"\";server=\"zk.de\";port=443;username=\"\";password=\"\";");

        String inputString = "";
        while (inputString.length() == 0)
        {
            if (Serial.available())
            {
                inputString = Serial.readStringUntil('\n');
            }
        }
        HelperUtils::parseConfigString(inputString, config);
        HelperUtils::saveConfigToEEPROM(config);
    }

    initKeyPins();

    LED_Strip.init();
    LED_Strip.setStaticColor("white");

    MAC_ADDRESS = HelperUtils::getMacAddress();

    modem.init();
    initTime();

    SPIFFSUtils::addLogEntry("ESP32 wird initialisiert");

    LED_Strip.setStaticColor("purple");
    modem.firmwareCheckAndUpdateIfNeeded();

    LED_Strip.setStaticColor("orange");

    int arraySize;
    String *rfids = modem.getRfids(arraySize);

    if (rfids != nullptr)
    {
        SPIFFSUtils::saveRfidsToSPIFFS(rfids, arraySize);
        delete[] rfids;
    }

    nfc.init();

    SPIFFSUtils::addLogEntry("ESP32 wurde fertig initialisiert");

    LED_Strip.setStaticColor("blue");
    modem.uploadLogs();

    LED_Strip.clear();
}

void loop()
{
    if (millis() >= targetMillis)
    {
        Serial.println("Target time reached.");
        Serial.println("Uploading logs and restarting ESP32...");
        SPIFFSUtils::addLogEntry("ESP32 wird neu gestartet");
        LED_Strip.setStaticColor("blue");
        while (!modem.uploadLogs())
        {
            delay(100);
        }
        ESP.restart(); // ESP32 neustarten
    }
    checkNFCTag();
}
