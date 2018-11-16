#include "Fx2.h"
#include "fx2regs.h"
#include "syncdly.h"            // SYNCDELAY macro

#include "USBDescriptors.h"
#include "USBSetup.h"
#include "i2c.h"

extern BOOL Rwuen;
extern BOOL Selfpwr;
BYTE Configuration; // Current configuration
BYTE AlternateSetting; // Alternate settings

// this table is used by the epcs macro
const char __code  EPCS_Offset_Lookup_Table[] =
{
   0,    // EP1OUT
   1,    // EP1IN
   2,    // EP2OUT
   2,    // EP2IN
   3,    // EP4OUT
   3,    // EP4IN
   4,    // EP6OUT
   4,    // EP6IN
   5,    // EP8OUT
   5,    // EP8IN
};
// macro for generating the address of an endpoint's control and status register (EPnCS)
#define epcs(EP) (EPCS_Offset_Lookup_Table[(EP & 0x7E) | (EP > 128)] + 0xE6A1)

// this function determines if the chip is high-speed capable.
// FX2 and FX2LP are high-speed capable. FX1 is not - it does
// not have a high-speed transceiver.
inline BOOL HighSpeedCapable()
{
   if (GPCR2 & bmFULLSPEEDONLY)
      return FALSE;
   else
      return TRUE;
}


USBSetupCallback GetDescriptor = DefaultGetDescriptor;
USBSetupCallback SetConfiguration = DefaultSetConfiguration;
USBSetupCallback GetConfiguration = DefaultGetConfiguration;
USBSetupCallback SetInterface = DefaultSetInterface;
USBSetupCallback GetInterface = DefaultGetInterface;
USBSetupCallback GetStatus = DefaultGetStatus;
USBSetupCallback ClearFeature = DefaultClearFeature;
USBSetupCallback SetFeature = DefaultSetFeature;
USBSetupCallback VendorCmnd = DefaultVendorCmnd;

void HandleSetupPacket();
void SetupCommand(void)
{
   if ((SETUPDAT[0] & SETUP_VENDOR_REQUEST) == SETUP_VENDOR_REQUEST)
   {
       if(!VendorCmnd()){
          EZUSB_STALL_EP0();            // Stall End Point 0
       }
   }
   else
   {
       HandleSetupPacket();
   }
   // Acknowledge handshake phase of device request
   EP0CS |= bmHSNAK;
}

void HandleSetupPacket()
{
    void   *dscr_ptr;
    switch(SETUPDAT[1])
    {
       case SC_GET_DESCRIPTOR:                  // *** Get Descriptor
          if(GetDescriptor())
             switch(SETUPDAT[3])
             {
                case GD_DEVICE:            // Device
                   SUDPTRH = MSB(pDeviceDscr);
                   SUDPTRL = LSB(pDeviceDscr);
                   break;
                case GD_DEVICE_QUALIFIER:            // Device Qualifier
                   // only retuen a device qualifier if this is a high speed
                   // capable chip.
                   if (HighSpeedCapable())
                   {
                       SUDPTRH = MSB(pDeviceQualDscr);
                       SUDPTRL = LSB(pDeviceQualDscr);
                   }
                   else
                   {
                       EZUSB_STALL_EP0();
                   }
                   break;
                case GD_CONFIGURATION:         // Configuration
                   SUDPTRH = MSB(pConfigDscr);
                   SUDPTRL = LSB(pConfigDscr);
                   break;
                case GD_OTHER_SPEED_CONFIGURATION:  // Other Speed Configuration
                   SUDPTRH = MSB(pOtherConfigDscr);
                   SUDPTRL = LSB(pOtherConfigDscr);
                   break;
                case GD_STRING:            // String
                   if( (dscr_ptr = GetStringDescriptor(SETUPDAT[2])) != 0)
                   {
                      SUDPTRH = MSB(dscr_ptr);
                      SUDPTRL = LSB(dscr_ptr);
                   }
                   else
                      EZUSB_STALL_EP0();   // Stall End Point 0
                   break;
                default:            // Invalid request
                   EZUSB_STALL_EP0();      // Stall End Point 0
             }
          break;
       case SC_GET_INTERFACE:                  // *** Get Interface
           GetInterface();
          break;
       case SC_SET_INTERFACE:                  // *** Set Interface
           SetInterface();
          break;
       case SC_SET_CONFIGURATION:               // *** Set Configuration
           SetConfiguration();
          break;
       case SC_GET_CONFIGURATION:               // *** Get Configuration
           GetConfiguration();
          break;
       case SC_GET_STATUS:                  // *** Get Status
          if(GetStatus())
             switch(SETUPDAT[0])
             {
                case GS_DEVICE:            // Device
                   EP0BUF[0] = ((BYTE)Rwuen << 1) | (BYTE)Selfpwr;
                   EP0BUF[1] = 0;
                   EP0BCH = 0;
                   EP0BCL = 2;
                   break;
                case GS_INTERFACE:         // Interface
                   EP0BUF[0] = 0;
                   EP0BUF[1] = 0;
                   EP0BCH = 0;
                   EP0BCL = 2;
                   break;
                case GS_ENDPOINT:         // End Point
                   EP0BUF[0] = *(BYTE __xdata *) epcs(SETUPDAT[4]) & bmEPSTALL;
                   EP0BUF[1] = 0;
                   EP0BCH = 0;
                   EP0BCL = 2;
                   break;
                default:            // Invalid Command
                   EZUSB_STALL_EP0();      // Stall End Point 0
             }
          break;
       case SC_CLEAR_FEATURE:                  // *** Clear Feature
          if(ClearFeature())
             switch(SETUPDAT[0])
             {
                case FT_DEVICE:            // Device
                   if(SETUPDAT[2] == 1)
                      Rwuen = FALSE;       // Disable Remote Wakeup
                   else
                      EZUSB_STALL_EP0();   // Stall End Point 0
                   break;
                case FT_ENDPOINT:         // End Point
                   if(SETUPDAT[2] == 0)
                   {
                      *(BYTE __xdata *) epcs(SETUPDAT[4]) &= ~bmEPSTALL;
                      EZUSB_RESET_DATA_TOGGLE( SETUPDAT[4] );
                   }
                   else
                      EZUSB_STALL_EP0();   // Stall End Point 0
                   break;
             }
          break;
       case SC_SET_FEATURE:                  // *** Set Feature
          if(SetFeature())
             switch(SETUPDAT[0])
             {
                case FT_DEVICE:            // Device
                   if(SETUPDAT[2] == 1)
                      Rwuen = TRUE;      // Enable Remote Wakeup
                   else if(SETUPDAT[2] == 2)
                      // Set Feature Test Mode.  The core handles this request.  However, it is
                      // necessary for the firmware to complete the handshake phase of the
                      // control transfer before the chip will enter test mode.  It is also
                      // necessary for FX2 to be physically disconnected (D+ and D-)
                      // from the host before it will enter test mode.
                      break;
                   else
                      EZUSB_STALL_EP0();   // Stall End Point 0
                   break;
                case FT_ENDPOINT:         // End Point
                   *(BYTE __xdata *) epcs(SETUPDAT[4]) |= bmEPSTALL;
                   break;
                default:
                   EZUSB_STALL_EP0();      // Stall End Point 0
             }
          break;
       default:                     // *** Invalid Command
          EZUSB_STALL_EP0();            // Stall End Point 0
          break;
    }

}

/// Called when a Get Descriptor command is received
BOOL DefaultGetDescriptor(void)
{
    return (TRUE);
}

/// Called when a Set Configuration command is received
BOOL DefaultSetConfiguration(void)
{
    Configuration = SETUPDAT[2];
    return (TRUE); // Handled by user code
}

/// Called when a Get Configuration command is received
BOOL DefaultGetConfiguration(void)
{
    EP0BUF[0] = Configuration;
    EP0BCH = 0;
    EP0BCL = 1;
    return (TRUE); // Handled by user code
}

/// Called when a Set Interface command is received
BOOL DefaultSetInterface(void)
{
    AlternateSetting = SETUPDAT[2];
    return (TRUE); // Handled by user code
}

/// Called when a Set Interface command is received
BOOL DefaultGetInterface(void)
{
    EP0BUF[0] = AlternateSetting;
    EP0BCH = 0;
    EP0BCL = 1;
    return (TRUE); // Handled by user code
}

BOOL DefaultGetStatus(void)
{
    return (TRUE);
}

BOOL DefaultClearFeature(void)
{
    return (TRUE);
}

BOOL DefaultSetFeature(void)
{
    return (TRUE);
}
#include "i2c.h"
BOOL DefaultVendorCmnd(void)
{
#ifdef _DEBUG
    i2c_print_string(0x8, "default\n");
#endif
    return (FALSE);
}
