#include <Fx2.h>
#include <fx2regs.h>
#include <syncdly.h>          // SYNCDELAY macro


#define ACTIVE_LOW

void SetupLEDs()
{
	OEA = 0xf;
	IOA = 0x0;
}
void SetLEDState(unsigned char index, char state)
{
	if (index >= 4)
		return;
#ifdef ACTIVE_LOW
	if (state == 0)
		IOA |= (0x1 << index); // set the bit
	else
		IOA &= ~(0x1 << index); // clear the bit
#else
	if (state == 0)
		IOA &= ~(0x1 << index); // clear the bit
	else
		IOA |= (0x1 << index); // set the bit
#endif
}
