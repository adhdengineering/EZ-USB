#include "Fx2.h"
#include "fx2regs.h"
#include "syncdly.h"            // SYNCDELAY macro

#include "delay.h"

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

#define NO_ASM_FUNCTIONS
#ifdef NO_ASM_FUNCTIONS
void Suspend()
{
	WAKEUPCS |= 0xc0;
	SUSPEND = 1;
	PCON |= 0x01;
	SYNCDELAY;
	SYNCDELAY;
	SYNCDELAY;
}
#else
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
	__endasm;
}
#endif


