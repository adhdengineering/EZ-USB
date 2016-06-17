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
                SetLEDState(0, 0);
                SetLEDState(1, 0);
                if ((i2cerr = i2c_read(SetupPacket.wValue, SetupPacket.wIndex, EP0BUF, SetupPacket.wLength)) == i2c_ok)
                {
                    EP0BCH = 0;
                    EP0BCL = SetupPacket.wLength;
                    return TRUE;
                }
                else
                {
                    switch (i2cerr)
                    {
                    case i2c_no_device_write_ack:
                        SetLEDState(0, 1);
                        SetLEDState(1, 1);
                        break;
                    case i2c_no_device_read_ack:
                        SetLEDState(0, 1);
                        SetLEDState(1, 0);
                        break;
                    case i2c_no_ack:
                        SetLEDState(0, 0);
                        SetLEDState(1, 1);
                        break;
                    default:
                        SetLEDState(0, 0);
                        SetLEDState(1, 0);
                        break;
                    }
                    return FALSE;
                }
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
