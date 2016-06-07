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

#endif /* SETUP_H_ */
