#ifndef SERIAL_H
#define SERIAL_H

#define COM1 0x3F8

void ser_init();
void ser_putc(char c);
void ser_puts(char* string);

#endif //SERIAL_H