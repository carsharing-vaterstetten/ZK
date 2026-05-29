#include "KeyControl.h"
#include "shared/RFIDs.h"

KeyControl::KeyControl(CarKeyDriver& driver, const RFIDs&, AccessStatus& accessStatus) : driver(driver),
    accessStatus(accessStatus) {}

void KeyControl::toggleLogin(const uint32_t rfid) const
{
    if (accessStatus.isLoggedIn())
    {
        driver.startCloseSequence();
        accessStatus.clrLoginData();
    }

    driver.startOpenSequence();
    accessStatus.setLoginData(rfid);
}
