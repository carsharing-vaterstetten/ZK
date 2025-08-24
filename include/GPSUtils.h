#include <Arduino.h>


#ifndef ZK_GPSUTILS_H
#define ZK_GPSUTILS_H


class GPSUtils
{
public:
    static void parseRawGPSDataString(String rawGPSData, float* UTCDateAndTime, float* latitude, float* longitude,
                                      float* mslAltitude, float* speedOverGround);
};


#endif //ZK_GPSUTILS_H
