#ifndef I2C_H_
#define I2C_H_

typedef enum
{
    i2c_ok = 0,
    i2c_error_flag,
    i2c_no_device_write_ack,
    i2c_no_device_read_ack,
    i2c_no_ack,
}i2c_error;

// Writes a string to the address. str must be null terminated.
i2c_error i2c_print_string(unsigned char addr, char* str);
i2c_error i2c_print_byte_as_hex(unsigned char addr, unsigned char data);
i2c_error i2c_write(unsigned char device, unsigned short addr, unsigned char* data, unsigned char len);
i2c_error i2c_read(unsigned char device, unsigned short addr, unsigned char* dst, unsigned char len);
#endif
