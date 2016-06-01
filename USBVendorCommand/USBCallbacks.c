#include <Fx2.h>
#include <fx2regs.h>
#include <syncdly.h>

BYTE Configuration; // Current configuration
BYTE AlternateSetting; // Alternate settings

#include <i2c.h>
#include "LEDs.h"

#define VR_NAKALL_ON    0xD0
#define VR_NAKALL_OFF   0xD1
#define LED_VENDOR_REQUEST 0x01

/// Called when a Get Descriptor command is received
BOOL DR_GetDescriptor(void)
{
    return (TRUE);
}

/// Called when a Set Configuration command is received
BOOL DR_SetConfiguration(void)
{
    Configuration = SETUPDAT[2];
    return (TRUE); // Handled by user code
}

/// Called when a Get Configuration command is received
BOOL DR_GetConfiguration(void)
{
    EP0BUF[0] = Configuration;
    EP0BCH = 0;
    EP0BCL = 1;
    return (TRUE); // Handled by user code
}

/// Called when a Set Interface command is received
BOOL DR_SetInterface(void)
{
    AlternateSetting = SETUPDAT[2];
    return (TRUE); // Handled by user code
}

/// Called when a Set Interface command is received
BOOL DR_GetInterface(void)
{
    EP0BUF[0] = AlternateSetting;
    EP0BCH = 0;
    EP0BCL = 1;
    return (TRUE); // Handled by user code
}

BOOL DR_GetStatus(void)
{
    return (TRUE);
}

BOOL DR_ClearFeature(void)
{
    return (TRUE);
}

BOOL DR_SetFeature(void)
{
    return (TRUE);
}

BOOL DR_VendorCmnd(void)
{
    BYTE tmp;

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
        case VR_NAKALL_ON:
            tmp = FIFORESET;
            tmp |= bmNAKALL;
            SYNCDELAY;
            FIFORESET = tmp;
            EP0BCH = 0;
            EP0BCL = 0;
            break;
        case VR_NAKALL_OFF:
            tmp = FIFORESET;
            tmp &= ~bmNAKALL;
            SYNCDELAY;
            FIFORESET = tmp;
            EP0BCH = 0;
            EP0BCL = 0;
            break;
        default:
            return (TRUE);
    }
    return (FALSE);
}
