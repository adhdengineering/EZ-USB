#include <Fx2.h>
#include <fx2regs.h>
#include <syncdly.h>          // SYNCDELAY macro


#define ACTIVE_LOW

void SetupLEDs()
{
	OEA = 0xf;
	IOA &= 0xF0;
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

char GetLEDState(unsigned char index)
{
    if (index >= 4)
        return 0;

#ifdef ACTIVE_LOW
    return (IOA & (0x1 << index)) ? 0 : 1;
#else
    return (IOA & (0x1 << index)) ? 1 : 0;
#endif
}
