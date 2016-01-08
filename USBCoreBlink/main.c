#include <Fx2.h>
#include <fx2regs.h>
#include <syncdly.h>            // SYNCDELAY macro

#include <USB.h>
#include <delay.h>
#include <USBsetup.h>
#include <SuspendResume.h>

#include "LEDs.h"


/*
//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
#define DELAY_COUNT   0x9248*8L  // Delay for 8 sec at 24Mhz, 4 sec at 48
#define _IFREQ  48000            // IFCLK constant for Synchronization Delay
#define _CFREQ  48000            // CLKOUT constant for Synchronization Delay

extern void Setup();
extern void Loop();
*/
BOOL TD_Suspend(void) // Called before the device goes into suspend mode
{
	return (TRUE);
}

BOOL TD_Resume(void) // Called after the device resumes
{
	return (TRUE);
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
				}while(!Rwuen && EZUSB_EXTWAKEUP());
				// above.  Must continue to go back into suspend if the host has disabled remote wakeup
				// *and* the wakeup was caused by the external wakeup pin.

				// 8051 activity will resume here due to USB bus or Wakeup# pin activity.
				Resume();   // If source is the Wakeup# pin, signal the host to Resume.
				TD_Resume();
			}
		}
	}
}
