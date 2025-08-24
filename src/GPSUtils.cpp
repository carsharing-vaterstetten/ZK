#include "../include/GPSUtils.h"


void GPSUtils::parseRawGPSDataString(String rawGPSData, float* UTCDateAndTime, float* latitude, float* longitude,
                           float* mslAltitude, float* speedOverGround)
{
    /*
    1. GNSS run status
    2. Fix status
    3. UTC date and time
    4. Latitude
    5. Longitude
    6. MSL altitude
    7. Speed over ground
    8. Course over ground
    9. Fix mode
    10. Reserver1
    11. HDOP
    12. PDOP
    13. VDOP
    14. Reserved2
    15. GNSS Satellites in View
    16. GPS Satellites used
    17. GLONASS Satellites used
    18. Reserver3
    19. C/N0 max
    20. HPA
    21. VPA

    example: 1,1,20220809173458.000,41.12XXXX,-8.52XXXX,140.200,0.00,237.6,1,,2.3,2.5,1.0,,20,5,1,,48,,
     */

    for (int i = 0; i < 21; ++i)
    {
        const int commaIndex = rawGPSData.indexOf(',');

        String value;
        if (commaIndex != -1)
        {
            value = rawGPSData.substring(0, commaIndex);
        }
        else
        {
            value = rawGPSData; // Last value (no comma)
        }

        switch (i)
        {
        case 2:
            if (UTCDateAndTime)
                *UTCDateAndTime = value.toFloat();
            break;
        case 3:
            if (latitude)
                *latitude = value.toFloat();
            break;
        case 4:
            if (longitude)
                *longitude = value.toFloat();
            break;
        case 5:
            if (mslAltitude)
                *mslAltitude = value.toFloat();
            break;
        case 6:
            if (speedOverGround)
                *speedOverGround = value.toFloat();
            break;
        default:
            break;
        }

        if (commaIndex == -1)
        {
            break; // No more values
        }

        rawGPSData = rawGPSData.substring(commaIndex + 1);
    }
}
