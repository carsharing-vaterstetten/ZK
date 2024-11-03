#include <Arduino.h>
#include <Modem.h>
#include <NFC.h>
#include <SPIFFSUtils.h>
#include <LED.h>

#define SerialMon Serial

bool loggedIn = false;
unsigned long targetMillis;  
const unsigned long dayMillis = 24 * 60 * 60 * 1000;  // Ein Tag in Millisekunden

Modem modem;
NFC nfc;
LED LED_Strip;

void initKeyPins()
{
    pinMode(OPEN_KEY, OUTPUT);
    pinMode(CLOSE_KEY, OUTPUT);
}

// Funktion überprüft, ob die gescannte Karte in der Speicherdatei vorhanden ist.
// Wenn ja, wird je nach Zustand loggedIn auf true oder false gesetzt.
void checkNFCTag()
{
    char *readValue = nfc.readTag();
    if (strcmp(readValue, "") != 0)
    {
        SerialMon.println(readValue);
        if (SPIFFSUtils::isRfidInSPIFFS(readValue))
        {
            loggedIn = !loggedIn;
            if (loggedIn)
            {
                digitalWrite(OPEN_KEY, HIGH);
                delay(200);
                LED_Strip.setStaticColor("green");
                SerialMon.println("AUF");
                digitalWrite(OPEN_KEY, LOW);
            }
            else
            {
                digitalWrite(CLOSE_KEY, HIGH);
                delay(200);
                LED_Strip.setStaticColor("red");
                SerialMon.println("ZU");
                digitalWrite(CLOSE_KEY, LOW);
            }
            delay(2000);
        }
        else
        {
            LED_Strip.setStaticColor("red");
            delay(2000);
        }
        LED_Strip.clear();
    }
    if (readValue != "")
    {
        delete[] readValue;
    }
}

void initTime(){

    String time = modem.getLocalTime();
    Serial.println("Local Time: " + time);

    // Zeitformat "HH:MM:SS+TZ".
    String hourStr = time.substring(0, 2);
    String minStr  = time.substring(3, 5);
    String secStr  = time.substring(6, 8);

    int hour   = hourStr.toInt();
    int minute = minStr.toInt();
    int second = secStr.toInt();

    // Berechne die Millisekunden seit Mitternacht
    unsigned long currentMillis = (hour * 3600 + minute * 60 + second) * 1000;

    // Berechne die verbleibende Zeit bis 4 Uhr morgens in Millisekunden
    if (currentMillis < targetTimeToRestartESP32) {
        targetMillis = targetTimeToRestartESP32 - currentMillis;
    } else {
        targetMillis = dayMillis - (currentMillis - targetTimeToRestartESP32);  // Nächster Tag 4 Uhr
    }

    targetMillis += millis();  // Setze Zielzeit relativ zu millis()    
}

void setup()
{
    SerialMon.begin(UART_BAUD);
    while (!SerialMon) {};

    initKeyPins();

    LED_Strip.init();
    LED_Strip.setStaticColor("white");

    modem.init();
    initTime();

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
    LED_Strip.clear();
}

void loop()
{
    if (millis() >= targetMillis) {
        Serial.println("Target time reached. Restarting ESP32...");
        ESP.restart();  // ESP32 neustarten
    }
    checkNFCTag();
}
