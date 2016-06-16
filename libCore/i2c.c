#include "Fx2.h"
#include "fx2regs.h"
#include "syncdly.h"          // SYNCDELAY macro

#include "i2c.h"
unsigned char get_nibble_as_hex(unsigned char nibble);
unsigned char i2c_write_address_to_device(unsigned char device, unsigned short addr);
unsigned char i2c_write_byte(unsigned char data);
unsigned char i2c_read_byte(unsigned char *data);
void i2c_stop();


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

    i2c_stop();

    return 1;
}

unsigned char i2c_write(unsigned char device, unsigned short addr, unsigned char* data, unsigned char len)
{
    unsigned char ret = 1;
    while (I2CS & bmSTOP);

    do
    {
        if (i2c_write_address_to_device(device, addr) == 0) { ret = 0; break; }

        while (len)
        {
            if (i2c_write_byte(*(data++)) == 0) { ret = 0; break; }
            len--;
        };

    } while(0);

    i2c_stop();

    return ret;
}

unsigned char i2c_read(unsigned char device, unsigned short addr, unsigned char* dst, unsigned char len)
{
    unsigned char ret = 1;
    while (I2CS & bmSTOP);

    do
    {
        if (i2c_write_address_to_device(device, addr) == 0) { ret = 0; break; }

        I2CS |= bmSTART;
        I2DAT = (device << 1) | 0x01;
        while (!(I2CS & bmDONE));
        if (I2CS & bmBERR) return 0;
        if ((I2CS & bmACK) == 0) break;

        *dst = I2DAT;
        while (len)
        {
            if (i2c_read_byte(dst++) == 0) { ret = 0; break; }
            len--;
        };
    } while(0);

    i2c_stop();

    return ret;
}

unsigned char i2c_write_address_to_device(unsigned char device, unsigned short addr)
{
    I2CS |= bmSTART;
    I2DAT = device << 1;

    while (!(I2CS & bmDONE));
    if (I2CS & bmBERR) return 0;
    if ((I2CS & bmACK) == 0) return 0;

    if (i2c_write_byte(addr & 0xff) == 0) return 0;
    if (i2c_write_byte((addr >> 8) & 0xff) == 0) return 0;

    return 1;
}

unsigned char i2c_write_byte(unsigned char data)
{
    I2DAT = data;
    while (!(I2CS & bmDONE));
    if (I2CS & bmBERR) return 0;
    if ((I2CS & bmACK) == 0) return 0;
    return 1;
}

unsigned char i2c_read_byte(unsigned char *data)
{
    while (!(I2CS & bmDONE));
    if (I2CS & bmBERR) return 0;
    *data = I2DAT;
    return 1;
}

void i2c_stop()
{
    I2CS |= bmSTOP;
    while ((I2CS & bmSTOP));
}
