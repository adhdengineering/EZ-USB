#include "Fx2.h"
#include "fx2regs.h"
#include "syncdly.h"            // SYNCDELAY macro

#include "USB.h"
#include "delay.h"
#include "LEDs.h"
#include "USBsetup.h"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
#define DELAY_COUNT   0x9248*8L  // Delay for 8 sec at 24Mhz, 4 sec at 48
#define _IFREQ  48000            // IFCLK constant for Synchronization Delay
#define _CFREQ  48000            // CLKOUT constant for Synchronization Delay

BOOL TD_Suspend(void) // Called before the device goes into suspend mode
{
	return (TRUE);
}

BOOL TD_Resume(void) // Called after the device resumes
{
	return (TRUE);
}

void Resume()
{
	if( ((WAKEUPCS & bmWUEN)&&(WAKEUPCS & bmWU)) ||   // Check status AND Enable
		((WAKEUPCS & bmWU2EN)&&(WAKEUPCS & bmWU2)) )
	{
		USBCS |= bmSIGRESUME;
		Delay(20);
		USBCS &= ~bmSIGRESUME;
	}
}
void Suspend()
{
	__asm
WAKEUPCS = 0xE682
SUSPEND = 0xE681
PCON = 0x87
   mov   dptr,#WAKEUPCS    // Clear the Wake Source bit(s) in
   movx  a,@dptr           // the WAKEUPCS register
   orl   a,#0xC0           // clear PA2 and WPIN
   movx  @dptr,a

   mov   dptr,#SUSPEND     //
   movx  @dptr,a           // write any walue to SUSPEND register

   orl   PCON,#0x01  // Place the processor in idle

   nop                     // Insert some meaningless instruction
   nop                     // fetches to insure that the processor
   nop                     // suspends and resumes before RET
   nop
   nop
er_end:     ret
	__endasm;
}
// Task dispatcher
void main(void)
{
	InitUSB();
	SetupLEDs();

	while (1)
	{
		PollUSB();
	      // check for and handle suspend.
	      // NOTE: Idle mode stops the processor clock.  There are only two
	      // ways out of idle mode, the WAKEUP pin, and detection of the USB
	      // resume state on the USB bus.  The timers will stop and the
	      // processor will not wake up on any other interrupts.
	      if (Sleep)
	      {
	         if(TD_Suspend())
	         {
	            Sleep = FALSE;     // Clear the "go to sleep" flag.  Do it here to prevent any race condition between wakeup and the next sleep.
	            do
	            {
	            	Suspend();         // Place processor in idle mode.
	            }
	            while(!Rwuen && EZUSB_EXTWAKEUP());
	            // above.  Must continue to go back into suspend if the host has disabled remote wakeup
	            // *and* the wakeup was caused by the external wakeup pin.

	            // 8051 activity will resume here due to USB bus or Wakeup# pin activity.
	            Resume();   // If source is the Wakeup# pin, signal the host to Resume.
	            TD_Resume();
	         }
	      }
	}
}
