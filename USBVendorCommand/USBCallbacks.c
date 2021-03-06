#include <Fx2.h>
#include <fx2regs.h>
#include <syncdly.h>

#include <i2c.h>
#include <USBSetup.h>
#include "LEDs.h"

#define LED_VENDOR_REQUEST 0x01
#define I2C_VENDOR_REQUEST 0x02

void TriggerEP0Buffer();

BOOL OnVendorCmnd(void)
{
    i2c_error i2cerr = i2c_ok;
    switch (SetupPacket.bRequest)
    {
        case LED_VENDOR_REQUEST:
            if (SetupPacket.direction == SetupDirectionHost)
            {
                EP0BUF[0] = GetLEDState(SetupPacket.wValue);
                EP0BCH = 0;
                EP0BCL = 1;
            }
            else
            {
                TriggerEP0Buffer();
                SetLEDState(SetupPacket.wValue, EP0BUF[0]);
            }
            return (TRUE);
        case I2C_VENDOR_REQUEST:
            if (SetupPacket.direction == SetupDirectionHost)
            {
                if ((i2cerr = i2c_read(SetupPacket.wValue, SetupPacket.wIndex, EP0BUF, SetupPacket.wLength)) == i2c_ok)
                {
                }
                else
                {
                    EP0BUF[0] = i2cerr;
                }
                EP0BCH = 0;
                EP0BCL = SetupPacket.wLength;
                return TRUE;
            }
            else
            {
                TriggerEP0Buffer();
                i2cerr = i2c_write(SetupPacket.wValue, SetupPacket.wIndex, EP0BUF, SetupPacket.wLength);
                return i2cerr == i2c_ok;
            }
        default:
            return (FALSE);
    }
}

void TriggerEP0Buffer()
{
    EP0BCH = 0;
    EP0BCL = 0;
    while(EP0CS & bmEPBUSY);
}
