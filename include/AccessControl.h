#pragma once

#include <Arduino.h>

class AccessControl
{
    static void unlockCar();
    static void lockCar();
public:
    static void init();
    static void login(uint32_t rfid);
    static void logout();
};
