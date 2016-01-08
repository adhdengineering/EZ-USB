#ifndef USBDESCRIPTORS_H_
#define USBDESCRIPTORS_H_


typedef struct
{
	BYTE   length;
	BYTE   type;
}DSCR;

typedef struct            // Device Descriptor
{
	BYTE   length;         // Descriptor length ( = sizeof(DEVICEDSCR) )
	BYTE   type;         // Decriptor type (Device = 1)
	BYTE   spec_ver_minor;   // Specification Version (BCD) minor
	BYTE   spec_ver_major;   // Specification Version (BCD) major
	BYTE   dev_class;      // Device class
	BYTE   sub_class;      // Device sub-class
	BYTE   protocol;      // Device sub-sub-class
	BYTE   max_packet;      // Maximum packet size
	WORD   vendor_id;      // Vendor ID
	WORD   product_id;      // Product ID
	WORD   version_id;      // Product version ID
	BYTE   mfg_str;      // Manufacturer string index
	BYTE   prod_str;      // Product string index
	BYTE   serialnum_str;   // Serial number string index
	BYTE   configs;      // Number of configurations
}DEVICEDSCR;

typedef struct            // Device Qualifier Descriptor
{
	BYTE   length;         // Descriptor length ( = sizeof(DEVICEQUALDSCR) )
	BYTE   type;         // Decriptor type (Device Qualifier = 6)
	BYTE   spec_ver_minor;   // Specification Version (BCD) minor
	BYTE   spec_ver_major;   // Specification Version (BCD) major
	BYTE   dev_class;      // Device class
	BYTE   sub_class;      // Device sub-class
	BYTE   protocol;      // Device sub-sub-class
	BYTE   max_packet;      // Maximum packet size
	BYTE   configs;      // Number of configurations
	BYTE  reserved0;
}DEVICEQUALDSCR;

typedef struct
{
	BYTE   length;         // Configuration length ( = sizeof(CONFIGDSCR) )
	BYTE   type;         // Descriptor type (Configuration = 2)
	WORD   config_len;      // Configuration + End Points length
	BYTE   interfaces;      // Number of interfaces
	BYTE   index;         // Configuration number
	BYTE   config_str;      // Configuration string
	BYTE   attrib;         // Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu
	BYTE   power;         // Power requirement (div 2 ma)
}CONFIGDSCR;

typedef struct
{
	BYTE   length;         // Interface descriptor length ( - sizeof(INTRFCDSCR) )
	BYTE   type;         // Descriptor type (Interface = 4)
	BYTE   index;         // Zero-based index of this interface
	BYTE   alt_setting;   // Alternate setting
	BYTE   ep_cnt;         // Number of end points
	BYTE   class;         // Interface class
	BYTE   sub_class;      // Interface sub class
	BYTE   protocol;      // Interface sub sub class
	BYTE   interface_str;   // Interface descriptor string index
}INTRFCDSCR;

typedef struct
{
	BYTE   length;         // End point descriptor length ( = sizeof(ENDPNTDSCR) )
	BYTE   type;         // Descriptor type (End point = 5)
	BYTE   addr;         // End point address
	BYTE   ep_type;      // End point type
	BYTE   mp_L;         // Maximum packet size
	BYTE   mp_H;
	BYTE   interval;      // Interrupt polling interval
}ENDPNTDSCR;

typedef struct
{
	BYTE   length;         // String descriptor length
	BYTE   type;         // Descriptor type
}STRINGDSCR;


extern WORD   pDeviceDscr;
extern WORD   pDeviceQualDscr;
extern WORD	  pHighSpeedConfigDscr;
extern WORD	  pFullSpeedConfigDscr;
extern WORD   pConfigDscr;
extern WORD   pOtherConfigDscr;
extern WORD   pStringDscr;
/*
extern __code DEVICEDSCR        DeviceDscr;
extern __code DEVICEQUALDSCR    DeviceQualDscr;
extern __code CONFIGDSCR        HighSpeedConfigDscr;
extern __code CONFIGDSCR        FullSpeedConfigDscr;
extern __code STRINGDSCR        StringDscr;
extern __code DSCR              UserDscr;
*/

void* GetStringDescriptorEx(unsigned char StrIdx);

#endif /* USBDESCRIPTORS_H_ */
