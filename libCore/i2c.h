#ifndef I2C_H_
#define I2C_H_

// Writes a string to the address. str must be null terminated.
unsigned char i2c_print_string(unsigned char addr, char* str);
unsigned char i2c_print_byte_as_hex(unsigned char addr, unsigned char data);
#endif
