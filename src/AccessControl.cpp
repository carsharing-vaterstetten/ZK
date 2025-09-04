#include "AccessControl.h"


#include "Config.h"
#include "Globals.h"
#include "LED.h"

void AccessControl::init()
{
    pinMode(OPEN_KEY, OUTPUT);
    pinMode(CLOSE_KEY, OUTPUT);
}

void AccessControl::lockCar()
{
    digitalWrite(CLOSE_KEY, HIGH);
    delay(200);
    digitalWrite(CLOSE_KEY, LOW);

    statusLed.setColor(Color::Red);
    fileLog.infoln("Car locked");
}


void AccessControl::unlockCar()
{
    digitalWrite(OPEN_KEY, HIGH);
    delay(200);
    digitalWrite(OPEN_KEY, LOW);
    statusLed.setColor(Color::Green);
    fileLog.infoln("Car unlocked");
}

void AccessControl::login(const uint32_t rfid)
{
    loggedInRFID = rfid;
    isLoggedIn = true;
    unlockCar();
}

void AccessControl::logout()
{
    loggedInRFID = 0;
    isLoggedIn = false;
    lockCar();
}
