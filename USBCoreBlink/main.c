#include <Fx2.h>
#include <fx2regs.h>
#include <syncdly.h>            // SYNCDELAY macro

#include <USB.h>
#include <delay.h>
#include <USBsetup.h>
#include <SuspendResume.h>

#include "LEDs.h"

void ProcessEP1Out()
{
	WORD i;

	SetLEDState(0, EP1OUTBUF[0]);
	SetLEDState(1, EP1OUTBUF[1]);
	SetLEDState(2, EP1OUTBUF[2]);
	SetLEDState(3, EP1OUTBUF[3]);

	for (i = 0; i < EP1OUTBC; i++)
	{
		EP1INBUF[i] = ~EP1OUTBUF[i];
	}
	EP1INBC = EP1OUTBC;
	EP1OUTBC = 0; // rearm the endpoint out buffer

}

BOOL TD_Suspend(void) // Called before the device goes into suspend mode
{
	return (TRUE);
}

BOOL TD_Resume(void) // Called after the device resumes
{
	return (TRUE);
}

// I'd like to move main into the core library, but it has to remain in the target project
// for now. I can't get the linker to include it in the output if it's in the library.
// Same as the USBJumpTable.asm file.
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

// Wake-up interrupt handler
void resume_isr(void) __interrupt(WKUP_VECT)
{
   EZUSB_CLEAR_RSMIRQ();
}
