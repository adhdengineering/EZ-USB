#ifndef SETUP_H_
#define SETUP_H_

extern BOOL Rwuen;
extern BOOL Selfpwr;

void SetupCommand(void);

// this is the definition of the callbacks used when there is a USB Setup packet.
// should return true if it handles the callback,
// false if not to return a nak to the usb host.
typedef BOOL (*USBSetupCallback)(void);

extern USBSetupCallback GetDescriptor;
extern USBSetupCallback SetConfiguration;
extern USBSetupCallback GetConfiguration;
extern USBSetupCallback SetInterface;
extern USBSetupCallback GetInterface;
extern USBSetupCallback GetStatus;
extern USBSetupCallback ClearFeature;
extern USBSetupCallback SetFeature;
extern USBSetupCallback VendorCmnd;

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

#endif /* SETUP_H_ */
