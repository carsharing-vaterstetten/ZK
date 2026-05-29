#pragma once

#include "drivers/car_key.h"
#include "shared/AccessStatus.h"
#include "shared/RFIDs.h"

class KeyControl
{
public:
    explicit KeyControl(CarKeyDriver& driver, const RFIDs& rfidsManager, AccessStatus& accessStatus);

    // Returns true if the rfid ended up logged in
    void toggleLogin(uint32_t rfid) const;

protected:
    CarKeyDriver& driver;
    AccessStatus& accessStatus;
};
