#include <Fx2.h>
#include <fx2regs.h>
#include <syncdly.h>

#include <i2c.h>
#include "LEDs.h"

#define LED_VENDOR_REQUEST 0x01
#define I2C_VENDOR_REQUEST 0x02

void TriggerEP0Buffer();

enum SetupDirection { SetupDirectionDevice=0, SetupDirectionHost=1 };
enum SetupType { SetupTypeStandard=0, SetupTypeClass=1, SetupTypeVendor=2, SetupTypeReserved=3 };
enum SetupRecipient { SetupRecipientDevice=0, SetupRecipientInterface=1, SetupRecipientEndpoint=2, SetupRecipientOther=3, SetupRecipientReserved=31 };

typedef struct __SetupPacket
{
    enum SetupRecipient recipient : 5;
    enum SetupType type : 2;
    enum SetupDirection direction : 1;
    unsigned char bRequest;
    unsigned short wValue;
    unsigned short wIndex;
    unsigned short wLength;
} _SetupPacket,*_PSetupPacket;

volatile __xdata __at (0xE6B8) volatile _SetupPacket SetupPacket;

BOOL OnVendorCmnd(void)
{
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
                i2c_print_byte_as_hex(0x08, SetupPacket.wLength);
                if (i2c_read(SetupPacket.wValue, SetupPacket.wIndex, EP0BUF, SetupPacket.wLength))
                {
                    EP0BCH = 0;
                    EP0BCL = SetupPacket.wLength;
                    return TRUE;
                }
                else { return FALSE; }
            }
            else
            {
                TriggerEP0Buffer();
                return i2c_write(SetupPacket.wValue, SetupPacket.wIndex, EP0BUF, SetupPacket.wLength);
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
