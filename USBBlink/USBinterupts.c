#pragma noiv               // Do not generate interrupt vectors

#include "Fx2.h"
#include "fx2regs.h"
#include "syncdly.h"            // SYNCDELAY macro

#include "USBsetup.h"

BOOL SetupDataAvailable;
BOOL Sleep;

//-----------------------------------------------------------------------------
// USB Interrupt Handlers
//   The following functions are called by the USB interrupt jump table.
//-----------------------------------------------------------------------------

// Setup Data Available Interrupt Handler
void ISR_SetupDataAvailable(void) __interrupt 0
{
	SetupDataAvailable = TRUE;            // Set flag
	EZUSB_IRQ_CLEAR();
	USBIRQ = bmSUDAV;         // Clear SUDAV IRQ
}

void ISR_StartOfFrame(void) __interrupt 0
{
	EZUSB_IRQ_CLEAR();
	USBIRQ = bmSOF;            // Clear SOF IRQ
}

// Setup Token __interrupt Handler
void ISR_SetupDataLoading(void) __interrupt 0
{
	EZUSB_IRQ_CLEAR();
	USBIRQ = bmSUTOK;         // Clear SUTOK IRQ
}

void ISR_Ures(void) __interrupt 0
{
	// whenever we get a USB reset, we should revert to full speed mode
	pConfigDscr = pFullSpeedConfigDscr;
	((CONFIGDSCR __xdata *) pConfigDscr)->type = CONFIG_DSCR;
	pOtherConfigDscr = pHighSpeedConfigDscr;
	((CONFIGDSCR __xdata *) pOtherConfigDscr)->type = OTHERSPEED_DSCR;

	EZUSB_IRQ_CLEAR();
	USBIRQ = bmURES;         // Clear URES IRQ
}

void ISR_Susp(void) __interrupt 0
{
	Sleep = TRUE;
	EZUSB_IRQ_CLEAR();
	USBIRQ = bmSUSP;
}

void ISR_Highspeed(void) __interrupt 0
{
	if (EZUSB_HIGHSPEED())
	{
	  pConfigDscr = pHighSpeedConfigDscr;
	  ((CONFIGDSCR __xdata *) pConfigDscr)->type = CONFIG_DSCR;
	  pOtherConfigDscr = pFullSpeedConfigDscr;
	  ((CONFIGDSCR __xdata *) pOtherConfigDscr)->type = OTHERSPEED_DSCR;
	}

	EZUSB_IRQ_CLEAR();
	USBIRQ = bmHSGRANT;
}
void ISR_Ep0ack(void) __interrupt 0
{
}
void ISR_Stub(void) __interrupt 0
{
}
void ISR_Ep0in(void) __interrupt 0
{
}
void ISR_Ep0out(void) __interrupt 0
{
}
void ISR_Ep1in(void) __interrupt 0
{
}
void ISR_Ep1out(void) __interrupt 0
{
}
void ISR_Ep2inout(void) __interrupt 0
{
}
void ISR_Ep4inout(void) __interrupt 0
{
}
void ISR_Ep6inout(void) __interrupt 0
{
}
void ISR_Ep8inout(void) __interrupt 0
{
}
void ISR_Ibn(void) __interrupt 0
{
}
void ISR_Ep0pingnak(void) __interrupt 0
{
}
void ISR_Ep1pingnak(void) __interrupt 0
{
}
void ISR_Ep2pingnak(void) __interrupt 0
{
}
void ISR_Ep4pingnak(void) __interrupt 0
{
}
void ISR_Ep6pingnak(void) __interrupt 0
{
}
void ISR_Ep8pingnak(void) __interrupt 0
{
}
void ISR_Errorlimit(void) __interrupt 0
{
}
void ISR_Ep2piderror(void) __interrupt 0
{
}
void ISR_Ep4piderror(void) __interrupt 0
{
}
void ISR_Ep6piderror(void) __interrupt 0
{
}
void ISR_Ep8piderror(void) __interrupt 0
{
}
void ISR_Ep2pflag(void) __interrupt 0
{
}
void ISR_Ep4pflag(void) __interrupt 0
{
}
void ISR_Ep6pflag(void) __interrupt 0
{
}
void ISR_Ep8pflag(void) __interrupt 0
{
}
void ISR_Ep2eflag(void) __interrupt 0
{
}
void ISR_Ep4eflag(void) __interrupt 0
{
}
void ISR_Ep6eflag(void) __interrupt 0
{
}
void ISR_Ep8eflag(void) __interrupt 0
{
}
void ISR_Ep2fflag(void) __interrupt 0
{
}
void ISR_Ep4fflag(void) __interrupt 0
{
}
void ISR_Ep6fflag(void) __interrupt 0
{
}
void ISR_Ep8fflag(void) __interrupt 0
{
}
void ISR_GpifComplete(void) __interrupt 0
{
}
void ISR_GpifWaveform(void) __interrupt 0
{
}
