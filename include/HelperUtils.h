// HelperUtils.h
#ifndef HELPERUTILS_H
#define HELPERUTILS_H

#include <Arduino.h>
#include <Config.h>
#include <Intern.h>
#include <WiFi.h>

class HelperUtils
{
public:
    static String getMacAddress();
    static String toUpperCase(const String &str);
};

#endif
