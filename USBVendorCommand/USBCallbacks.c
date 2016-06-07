#include <Fx2.h>
#include <fx2regs.h>
#include <syncdly.h>

#include <i2c.h>
#include "LEDs.h"

#define LED_VENDOR_REQUEST 0x01

BOOL OnVendorCmnd(void)
{
    //BYTE tmp;

    switch (SETUPDAT[1])
    {
        case LED_VENDOR_REQUEST:
            if ((SETUPDAT[0] & 0x80) == 0x80)
            {
                EP0BUF[0] = GetLEDState(SETUPDAT[2]);
                i2c_print_byte_as_hex(0x8, EP0BUF[0]);
                i2c_print_string(0x8, "g\n");
                EP0BCH = 0;
                EP0BCL = 1;
            }
            else
            {
                EP0BCH = 0;
                EP0BCL = 0;
                while(EP0CS & bmEPBUSY);
                i2c_print_byte_as_hex(0x8, EP0BUF[0]);
                i2c_print_string(0x8, "s\n");
                SetLEDState(SETUPDAT[2], EP0BUF[0]);
            }
            break;
        default:
            return (FALSE);
    }
    return (TRUE);
}
