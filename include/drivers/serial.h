#ifndef __drivers__serial__
#define __drivers__serial__

#include <types.h>

#define PORT_COM1 0x3f8

extern int serial_init();
extern void serial_write(char ch);
extern void serial_writeline(char * str);

#endif