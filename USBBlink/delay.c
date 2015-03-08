#include "Fx2.h"
#include "fx2regs.h"

void Delay1ms()
{
	__asm
	// Delay for 1 millisecond (1000 microseconds).
	// 10 cycles * 166.6 ns per cycle is 1.66 microseconds per loop.
	// 1000 microseconds / 1.66 = 602.  [assumes 24 MHz clock]
		mov	a, #0			// Clear dps so that we're using dph and dpl!
		mov	_DPS, a			//
		mov	dptr,#(0xffff - 602) 	// long pulse for operating
		mov	r4,#5

_loop:
		inc     dptr        // 3 cycles
		mov     a,dpl       // 2 cycles
        orl     a,dph       // 2 cycles
        jnz     _loop		// 3 cycles
	__endasm;
}

void Delay(WORD ms)
{
   if ((CPUCS & bmCLKSPD) == 0)              // 12Mhz
      ms = (ms + 1) / 2;                     // Round up before dividing so we can accept 1.
   else if ((CPUCS & bmCLKSPD) == bmCLKSPD1)   // 48Mhz
      ms = ms * 2;

	while(ms--)
		Delay1ms();
}
