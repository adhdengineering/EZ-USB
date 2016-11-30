#include "Fx2.h"
#include "fx2regs.h"
#include "syncdly.h"          // SYNCDELAY macro

#include "i2c.h"

char get_nibble_as_hex(unsigned char nibble);
i2c_error i2c_write_address_to_device(unsigned char device, unsigned char addr);
i2c_error i2c_write_byte(unsigned char data);
i2c_error i2c_read_byte(unsigned char *data, int bytesLeftAfterThisOne);
void i2c_stop();


i2c_error i2c_print_byte_as_hex(unsigned char addr, unsigned char byte)
{
    char temp[3];
    temp[0] = get_nibble_as_hex(byte >> 4);
    temp[1] = get_nibble_as_hex(byte);
    temp[2] = '\0';
    return i2c_print_string(addr, temp);
}

char get_nibble_as_hex(unsigned char nibble)
{
    nibble &= 0xf;
    if (nibble <= 9) return '0' + nibble;
    if (nibble <= 0xf) return 'A' + nibble - 10;
    return 'x';
}

i2c_error i2c_print_string(unsigned char addr, char* str)
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

i2c_error i2c_write(unsigned char device, unsigned char reg, unsigned char* data, unsigned char len)
{
    i2c_error ret = i2c_ok;
    while (I2CS & bmSTOP);

    do
    {
        if ((ret = i2c_write_address_to_device(device, reg)) != i2c_ok) { break; }

        while (len--)
        {
            if ((ret = i2c_write_byte(*(data++))) != i2c_ok) { break; }
        };

    } while(0);

    i2c_stop();

    return ret;
}

// From Section 13.5.4 of the FX2LP Technical Reference Manual.
//1. Set START=1. If BERR = 1, start timer. The timeout should be at least as long as the longest expected Start-to-Stop interval on the bus.
//2. Write the 7-bit peripheral address and the direction bit (1 for a read) to I2DAT.
//3. Wait for DONE=1 or for timer to expire*. If BERR=1, go to step 1.
//4. If ACK=0, set STOP=1 and go to step 15.
//5. Read I2DAT to initiate the first burst of nine SCL pulses to clock in the first byte from the slave. Discard the value that was read from I2DAT.
//6. Wait for DONE=1. If BERR=1, go to step 1.
//7. Read the just-received byte of data from I2DAT. This read also initiates the next read transfer.
//8. Repeat steps 6 and 7 for each byte until ready to read the second-to-last byte.
//9. Wait for DONE=1. If BERR=1, go to step 1.
//10. Before reading the second-to-last I2DAT byte, set LASTRD=1.
//11. Read the second-to-last byte from I2DAT. With LASTRD=1, this initiates the final byte read on the bus.
//12. Wait for DONE=1. If BERR=1, go to step 1.
//13. Set STOP=1.
//14. Read the final byte from I2DAT immediately (the next instruction) after setting the STOP bit. By reading I2DAT while the
    //‘stop’ condition is being generated, the just received data byte is retrieved without initiating an extra read transaction
    //(nine more SCL pulses) on the I2Cbus.
//15. Wait for STOP = 0 before initiating another transfer.
i2c_error i2c_read(unsigned char device, unsigned char reg, unsigned char* dst, unsigned char len)
{
    i2c_error ret = i2c_ok;
    while (I2CS & bmSTOP);
    do
    {
        unsigned char discard;
        if ((ret = i2c_write_address_to_device(device, reg)) != i2c_ok) {  break; }

        I2CS |= bmSTART;
        I2DAT = (device << 1) | 0x01;
        while (!(I2CS & bmDONE));
        if (I2CS & bmBERR) return i2c_error_flag;
        if ((I2CS & bmACK) == 0) { ret = i2c_no_device_read_address_ack; break; }

        discard = I2DAT;
        while (len--)
        {
            if ((ret = i2c_read_byte(dst++, len)) != i2c_ok) { break; }
        };
        return i2c_ok;
    } while(0);

    i2c_stop();

    return ret;
}

i2c_error i2c_write_address_to_device(unsigned char device, unsigned char addr)
{
    i2c_error ret = i2c_ok;
    I2CS |= bmSTART;
    I2DAT = device << 1;

    while (!(I2CS & bmDONE));
    if (I2CS & bmBERR) return i2c_error_flag;
    if ((I2CS & bmACK) == 0) return i2c_no_device_write_address_ack;

    if ((ret = i2c_write_byte(addr)) != i2c_ok) return ret;

    return i2c_ok;
}

i2c_error i2c_write_byte(unsigned char data)
{
    I2DAT = data;
    while (!(I2CS & bmDONE));
    if (I2CS & bmBERR) return i2c_error_flag;
    if ((I2CS & bmACK) == 0) return i2c_no_device_write_data_ack;
    return i2c_ok;
}

i2c_error i2c_read_byte(unsigned char *data, int bytesLeftAfterThisOne)
{
    while (!(I2CS & bmDONE));
    if (I2CS & bmBERR) return i2c_error_flag;
    if (bytesLeftAfterThisOne == 1)  I2CS |= bmLASTRD;
    if (bytesLeftAfterThisOne == 0)  I2CS |= bmSTOP;
    *data = I2DAT;
    return i2c_ok;
}

void i2c_stop()
{
    I2CS |= bmSTOP;
    while ((I2CS & bmSTOP));
}
