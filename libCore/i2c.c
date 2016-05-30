#include "Fx2.h"
#include "fx2regs.h"
#include "syncdly.h"          // SYNCDELAY macro

unsigned char i2c_print_string(unsigned char addr, char* str)
{
    while (I2CS & bmSTOP);

    I2CS |= bmSTART;
    I2DAT = addr << 1;

    while (!(I2CS & bmDONE));
    if (I2CS & bmBERR) { return 0;}
    if ((I2CS & bmACK) == 0) { return 0; }

    do
    {
        I2DAT = *str;
        str++;
        while (!(I2CS & bmDONE));
        if (I2CS & bmBERR) { return 0; }
        if ((I2CS & bmACK) == 0) return 0;
    } while (*str != 0);

    I2CS |= bmSTOP;
    while ((I2CS & bmSTOP));

    return 1;
}
