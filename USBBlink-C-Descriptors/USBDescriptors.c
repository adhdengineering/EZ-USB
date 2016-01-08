/*
 * USBDesciptors.c
 *
 *  Created on: 5 Jan 2016
 *      Author: c.greer
 */

#include "Fx2.h"

#define DSCR_DEVICE  1
#define DSCR_CONFIG  2
#define DSCR_STRING  3
#define DSCR_INTRFC  4
#define DSCR_ENDPNT  5
#define DSCR_DEVQUAL 6

#define ET_CONTROL 0
#define ET_ISO     1
#define ET_BULK    2
#define ET_INT     3

typedef struct
{
	CONFIGDSCR ConfigDsc;
	INTRFCDSCR InterfaceDescriptor;
	ENDPNTDSCR EndPointDescriptors[2];
}CONFIGANDINTERFACEDSCR;

typedef short wchar_t;
typedef struct
{
    STRINGDSCR StringDesciptor;
    wchar_t strData[];
}STRING;


#define START_ADDRESS 0x0400
// Makes a string descriptor length from a string. takes away 2 because the string will be null terminated but
// the descriptor isn't.
#define MAKE_STRINGDSCR_LEN(x) ( sizeof(STRINGDSCR) + (sizeof(x) * 2) - 2)
#define MAKE_EVEN_ALIGNED_START(start, type) (start + sizeof(type) + (sizeof(type) % 2))

#define DeviceDscr_Addr (START_ADDRESS)
#define DeviceQualDscr_Addr MAKE_EVEN_ALIGNED_START(DeviceDscr_Addr, DEVICEDSCR)
#define HighSpeedConfigDscr_Addr MAKE_EVEN_ALIGNED_START(DeviceQualDscr_Addr, DEVICEQUALDSCR)
#define FullSpeedConfigDscr_Addr MAKE_EVEN_ALIGNED_START(HighSpeedConfigDscr_Addr, CONFIGANDINTERFACEDSCR)
#define LanguageID_Addr MAKE_EVEN_ALIGNED_START(FullSpeedConfigDscr_Addr, CONFIGANDINTERFACEDSCR)
#define Manufacturer_Addr (LanguageID_Addr + MAKE_STRINGDSCR_LEN("a"))
#define Product_Addr (Manufacturer_Addr + MAKE_STRINGDSCR_LEN("Cypress"))


const DEVICEDSCR __at(DeviceDscr_Addr) DeviceDscr =
{
		.length = sizeof(DEVICEDSCR), // Descriptor length
		.type = DSCR_DEVICE, // Descriptor type
		.spec_ver_minor = 0x00, // Specification Version (BCD) minor
		.spec_ver_major = 0x02, // Specification Version (BCD) major
		.dev_class = 0x00, // Device class
		.sub_class = 0x00, // Device sub-class
		.protocol = 0x00, // Device sub-sub-class
		.max_packet = 64,  // Maximum packet size
		.vendor_id = 0x04B4, // Vendor ID
		.product_id = 0x8613, //Product ID (Sample Device)
		.version_id = 0x0000, //Product version ID
		.mfg_str = 1, // Manufacturer string index
		.prod_str = 2, // Product string index
		.serialnum_str = 0, // Serial number string index
		.configs = 1, // Number of configurations
};

const DEVICEQUALDSCR __at(DeviceQualDscr_Addr) DeviceQualDscr =          // Device Qualifier Descriptor
{
   .length = sizeof(DEVICEQUALDSCR),         // Descriptor length
   .type = DSCR_DEVQUAL,         // Descriptor type (Device Qualifier = 6)
   .spec_ver_minor = 0x00,   // Specification Version (BCD) minor
   .spec_ver_major = 0x02,   // Specification Version (BCD) major
   .dev_class = 0x00,      // Device class
   .sub_class = 0x00,      // Device sub-class
   .protocol = 0x00,      // Device sub-sub-class
   .max_packet = 64,      // Maximum packet size
   .configs = 1,      // Number of configurations
   .reserved0 = 0,
};

const CONFIGANDINTERFACEDSCR __at(HighSpeedConfigDscr_Addr) HighSpeedConfigDscr =
{
	.ConfigDsc =
	{
	   .length = sizeof(CONFIGDSCR),         // Configuration length ( = sizeof(CONFIGDSCR) )
	   .type = DSCR_CONFIG,         // Descriptor type (Configuration = 2)
	   .config_len = sizeof(CONFIGANDINTERFACEDSCR),      // Configuration + End Points length
	   .interfaces = 1,      // Number of interfaces
	   .index = 1,         // Configuration number
	   .config_str = 0,      // Configuration string
	   .attrib = 0x80,         // Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu
	   .power = 50,         // Power requirement (div 2 ma)
	},
	.InterfaceDescriptor =
	{
		.length = sizeof(INTRFCDSCR),         // Interface descriptor length ( - sizeof(INTRFCDSCR) )
		.type = DSCR_INTRFC,         // Descriptor type (Interface = 4)
		.index = 0,         // Zero-based index of this interface
		.alt_setting = 0,   // Alternate setting
		.ep_cnt = 2,         // Number of end points
		.class = 0xff,         // Interface class
		.sub_class = 0,      // Interface sub class
		.protocol = 0,      // Interface sub sub class
		.interface_str = 0,   // Interface descriptor string index
	},
	.EndPointDescriptors =
	{
		{
			.length = sizeof(ENDPNTDSCR),         // End point descriptor length ( = sizeof(ENDPNTDSCR) )
			.type = DSCR_ENDPNT,         // Descriptor type (End point = 5)
			.addr = 0x1,         // End point address
			.ep_type = ET_BULK,      // End point type
			.mp_L = 0x40,         // Maximum packet size
			.mp_H = 0x0,
			.interval = 0x00,      // Interrupt polling interval
		},
		{
			.length = sizeof(ENDPNTDSCR),         // End point descriptor length ( = sizeof(ENDPNTDSCR) )
			.type = DSCR_ENDPNT,         // Descriptor type (End point = 5)
			.addr = 0x81,         // End point address
			.ep_type = ET_BULK,      // End point type
			.mp_L = 0x40,         // Maximum packet size
			.mp_H = 0x0,
			.interval = 0x00,      // Interrupt polling interval
		},
	},
};

const CONFIGANDINTERFACEDSCR __at(FullSpeedConfigDscr_Addr) FullSpeedConfigDscr =
{
	.ConfigDsc =
	{
	   .length = sizeof(CONFIGDSCR),         // Configuration length ( = sizeof(CONFIGDSCR) )
	   .type = DSCR_CONFIG,         // Descriptor type (Configuration = 2)
	   .config_len = sizeof(CONFIGANDINTERFACEDSCR),      // Configuration + End Points length
	   .interfaces = 1,      // Number of interfaces
	   .index = 1,         // Configuration number
	   .config_str = 0,      // Configuration string
	   .attrib = 0x80,         // Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu
	   .power = 50,         // Power requirement (div 2 ma)
	},
	.InterfaceDescriptor =
	{
		.length = sizeof(INTRFCDSCR),         // Interface descriptor length ( - sizeof(INTRFCDSCR) )
		.type = DSCR_INTRFC,         // Descriptor type (Interface = 4)
		.index = 0,         // Zero-based index of this interface
		.alt_setting = 0,   // Alternate setting
		.ep_cnt = 2,         // Number of end points
		.class = 0xff,         // Interface class
		.sub_class = 0,      // Interface sub class
		.protocol = 0,      // Interface sub sub class
		.interface_str = 0,   // Interface descriptor string index
	},
	.EndPointDescriptors =
	{
		{
			.length = sizeof(ENDPNTDSCR),         // End point descriptor length ( = sizeof(ENDPNTDSCR) )
			.type = DSCR_ENDPNT,         // Descriptor type (End point = 5)
			.addr = 0x1,         // End point address
			.ep_type = ET_BULK,      // End point type
			.mp_L = 0x40,         // Maximum packet size
			.mp_H = 0x0,
			.interval = 0x00,      // Interrupt polling interval
		},
		{
			.length = sizeof(ENDPNTDSCR),         // End point descriptor length ( = sizeof(ENDPNTDSCR) )
			.type = DSCR_ENDPNT,         // Descriptor type (End point = 5)
			.addr = 0x81,         // End point address
			.ep_type = ET_BULK,      // End point type
			.mp_L = 0x40,         // Maximum packet size
			.mp_H = 0x0,
			.interval = 0x00,      // Interrupt polling interval
		},
	},
};
const STRINGDSCR __at(LanguageID_Addr) StringDscr;

const STRING __at(LanguageID_Addr) LanguageID =
{
	.StringDesciptor =
	{
		.length = sizeof(STRINGDSCR) + 2,
		.type = DSCR_STRING,
	},
	.strData = { 0x0409 },
};

const STRING __at(Manufacturer_Addr) Manufacturer =
{
	.StringDesciptor =
	{
		.length = MAKE_STRINGDSCR_LEN("Cypress"),
		.type = DSCR_STRING,
	},
	.strData = { 'C', 'y', 'p', 'r', 'e', 's', 's' },
};

const STRING __at(Product_Addr) Product =
{
	.StringDesciptor =
	{
		.length = MAKE_STRINGDSCR_LEN("EZ-USB"),
		.type = DSCR_STRING,
	},
	.strData = { 'E', 'Z', '-', 'U', 'S', 'B' },
};

//#define USE_ARRAY_FOR_STRING_TABLE
#ifdef USE_ARRAY_FOR_STRING_TABLE
const STRINGDSCR __xdata* StringTable[] =
{
	(STRINGDSCR __xdata*)&LanguageID,
	(STRINGDSCR __xdata*)&Manufacturer,
	(STRINGDSCR __xdata*)&Product,
};
#endif

void* GetStringDescriptorEx(BYTE StrIdx)
{
#ifdef USE_ARRAY_FOR_STRING_TABLE
	const int temp = sizeof(StringTable) / sizeof(STRINGDSCR __xdata *);
	if (StrIdx >= temp) return (void*)0;
	return StringTable[StrIdx];
#else
	switch (StrIdx)
	{
	case 0:
		return (void*)&LanguageID.StringDesciptor;
	case 1:
		return (void*)&Manufacturer.StringDesciptor;
	case 2:
		return (void*)&Product.StringDesciptor;
	}
	return (void*)0;
#endif
}
