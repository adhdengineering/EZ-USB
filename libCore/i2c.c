#include "Fx2.h"
#include "fx2regs.h"
#include "syncdly.h"          // SYNCDELAY macro

#include "i2c.h"
unsigned char get_nibble_as_hex(unsigned char nibble);

unsigned char i2c_print_byte_as_hex(unsigned char addr, unsigned char byte)
{
    char temp[3];
    temp[0] = get_nibble_as_hex(byte >> 4);
    temp[1] = get_nibble_as_hex(byte);
    temp[2] = '\0';
    return i2c_print_string(addr, temp);
}

unsigned char get_nibble_as_hex(unsigned char nibble)
{
    nibble &= 0xf;
    if (nibble <= 9) return '0' + nibble;
    if (nibble <= 0xf) return 'A' + nibble - 10;
    return 'x';
}

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
