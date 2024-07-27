#include <Arduino.h>
#include <Modem.h>
#include <NFC.h>
#include <SPIFFSUtils.h>
#include <LED.h>

#define SerialMon Serial

bool loggedIn = false;

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

void setup()
{
    SerialMon.begin(UART_BAUD);

    initKeyPins();

    LED_Strip.init();
    LED_Strip.setStaticColor("orange");

    modem.powerOn();

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
    checkNFCTag();
}
