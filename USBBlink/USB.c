#include "Fx2.h"
#include "fx2regs.h"
#include "syncdly.h"            // SYNCDELAY macro

#include "delay.h"
#include "LEDs.h"
#include "USBsetup.h"
#include "USBinterupts.h"

WORD   pDeviceDscr;   // Pointer to Device Descriptor; Descriptors may be moved
WORD   pDeviceQualDscr;
WORD   pHighSpeedConfigDscr;
WORD   pFullSpeedConfigDscr;
WORD   pConfigDscr;
WORD   pOtherConfigDscr;
WORD   pStringDscr;

void EZUSB_Discon(BOOL renum);



void InitUSB()
{
	SetupDataAvailable = FALSE;
	// set the CPU clock to 48MHz
	CPUCS = ((CPUCS & ~bmCLKSPD) | bmCLKSPD1);

	pDeviceDscr = (WORD)&DeviceDscr;
	pDeviceQualDscr = (WORD)&DeviceQualDscr;
	pHighSpeedConfigDscr = (WORD)&HighSpeedConfigDscr;
	pFullSpeedConfigDscr = (WORD)&FullSpeedConfigDscr;
	pStringDscr = (WORD)&StringDscr;

	EZUSB_IRQ_ENABLE();            // Enable USB interrupt (INT2)
	EZUSB_ENABLE_RSMIRQ();            // Wake-up interrupt

	INTSETUP |= (bmAV2EN | bmAV4EN);     // Enable INT 2 & 4 autovectoring

	USBIE |= bmSUDAV | bmSUTOK | bmSUSP | bmURES | bmHSGRANT;   // Enable selected interrupts
	EA = 1;                  // Enable 8051 interrupts

#ifndef NO_RENUM
   // Renumerate if necessary.  Do this by checking the renum bit.  If it
   // is already set, there is no need to renumerate.  The renum bit will
   // already be set if this firmware was loaded from an eeprom.
   if(!(USBCS & bmRENUM))
   {
       EZUSB_Discon(TRUE);   // renumerate
   }
#endif

   // unconditionally re-connect.  If we loaded from eeprom we are
   // disconnected and need to connect.  If we just renumerated this
   // is not necessary but doesn't hurt anything
   USBCS &=~bmDISCON;

   CKCON = (CKCON&(~bmSTRETCH)) | FW_STRETCH_VALUE; // Set stretch

   // clear the Sleep flag.
   Sleep = FALSE;

   EP1OUTBC = 4;
}

void PollUSB()
{
	WORD i;
    // Check for pending SETUP
    if(SetupDataAvailable)
    {
       SetupCommand();          // Implement setup command
       SetupDataAvailable = FALSE;          // Clear SETUP flag
    }

    // if the busy flag is set then the endpoint1 outbuffer is "owned" by the USB serial interface engine
    // if the bit is cleared then there is data in the buffer of length EP1OUTBC we can use.
    // we need to write to EP1OUTBC to flag the buffer as done and transfer ownership back to the SIE
	if (!(EP1OUTCS & bmEPBUSY))
	{
		SetLEDState(0, EP1OUTBUF[0]);
		SetLEDState(1, EP1OUTBUF[1]);
		SetLEDState(2, EP1OUTBUF[2]);
		SetLEDState(3, EP1OUTBUF[3]);

		for (i = 0; i < EP1OUTBC; i++)
		{
			EP1INBUF[i] = ~EP1OUTBUF[i];
		}
		EP1INBC = EP1OUTBC;
		EP1OUTBC = 4;
	}

/*
	if (!(EP2468STAT & bmEP2EMPTY)) { // check EP2 EMPTY(busy) bit in EP2468STAT (SFR), core set's this bit when FIFO is empty
		if (!(EP2468STAT & bmEP6FULL)) { // check EP6 FULL(busy) bit in EP2468STAT (SFR), core set's this bit when FIFO is full
			AUTOPTRH1 = MSB( &EP2FIFOBUF );
			AUTOPTRL1 = LSB( &EP2FIFOBUF );

			AUTOPTRH2 = MSB( &EP6FIFOBUF );
			AUTOPTRL2 = LSB( &EP6FIFOBUF );

			count = (EP2BCH << 8) + EP2BCL;

			// loop EP2OUT buffer data to EP6IN
			for (i = 0x0000; i < count; i++) {
				// setup to transfer EP2OUT buffer to EP6IN buffer using AUTOPOINTER(s)
				XAUTODAT2 = XAUTODAT1;
			}
			EP6BCH = EP2BCH;
			SYNCDELAY;
			EP6BCL = EP2BCL; // arm EP6IN
			SYNCDELAY;
			EP2BCL = 0x80; // re(arm) EP2OUT
		}
	}

	if (!(EP2468STAT & bmEP4EMPTY)) { // check EP4 EMPTY(busy) bit in EP2468STAT (SFR), core set's this bit when FIFO is empty
		if (!(EP2468STAT & bmEP8FULL)) { // check EP8 FULL(busy) bit in EP2468STAT (SFR), core set's this bit when FIFO is full
			APTR1H = MSB( &EP4FIFOBUF );
			APTR1L = LSB( &EP4FIFOBUF );

			AUTOPTRH2 = MSB( &EP8FIFOBUF );
			AUTOPTRL2 = LSB( &EP8FIFOBUF );

			count = (EP4BCH << 8) + EP4BCL;

			// loop EP4OUT buffer data to EP8IN
			for (i = 0x0000; i < count; i++) {
				// setup to transfer EP4OUT buffer to EP8IN buffer using AUTOPOINTER(s)
				EXTAUTODAT2 = EXTAUTODAT1;
			}
			EP8BCH = EP4BCH;
			SYNCDELAY;
			EP8BCL = EP4BCL; // arm EP8IN
			SYNCDELAY;
			EP4BCL = 0x80; // re(arm) EP4OUT
		}
	}*/
}


void EZUSB_Discon(BOOL renum)
{
   if(renum)                                 // If renumerate (i.e. 8051 will handle SETUP commands)
      USBCS |= (bmDISCON | bmRENUM);        // disconnect from USB and set the renumerate bit
   else
      USBCS |= bmDISCON;                     // just disconnect from USB

   Delay(1500);      // Wait 1500 ms

   USBIRQ = 0xff;          // Clear any pending USB interrupt requests.  They're for our old life.
   EPIRQ = 0xff;
   EZUSB_IRQ_CLEAR();

   USBCS &=~bmDISCON;      // reconnect USB

}

