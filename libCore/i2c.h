#ifndef I2C_H_
#define I2C_H_

// Writes a string to the address. str must be null terminated.
unsigned char i2c_print_string(unsigned char addr, char* str);

#endif
