#ifndef USBDESCRIPTORS_H_
#define USBDESCRIPTORS_H_

void* GetStringDescriptorEx(unsigned char StrIdx);

extern WORD pDeviceDscr;
extern WORD pDeviceQualDscr;
extern WORD pHighSpeedConfigDscr;
extern WORD pFullSpeedConfigDscr;
extern WORD pConfigDscr;
extern WORD pOtherConfigDscr;

//extern WORD   pStringDscr;
/*
extern __code DEVICEDSCR        DeviceDscr;
extern __code DEVICEQUALDSCR    DeviceQualDscr;
extern __code CONFIGDSCR        HighSpeedConfigDscr;
extern __code CONFIGDSCR        FullSpeedConfigDscr;
extern __code STRINGDSCR        StringDscr;
extern __code DSCR              UserDscr;
*/

#endif /* USBDESCRIPTORS_H_ */
