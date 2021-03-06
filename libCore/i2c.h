#ifndef I2C_H_
#define I2C_H_

typedef enum
{
    i2c_ok = 0,
    i2c_error_flag,
    i2c_no_device_write_address_ack,
    i2c_no_device_write_data_ack,
    i2c_no_device_read_address_ack,
    i2c_no_device_read_data_ack,
    i2c_error_0 = 0x20,
    i2c_error_1 = 0x11,
    i2c_error_2 = 0x12,
    i2c_error_3 = 0x13,
    i2c_error_4 = 0x14,
}i2c_error;


// Writes a string to the address. str must be null terminated.
i2c_error i2c_print_string(unsigned char addr, char* str);
i2c_error i2c_print_byte_as_hex(unsigned char addr, unsigned char data);

i2c_error i2c_write(unsigned char device, unsigned char reg, unsigned char* data, unsigned char len);
i2c_error i2c_read(unsigned char device, unsigned char reg, unsigned char* dst, unsigned char len);
#endif
