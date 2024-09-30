#ifndef __stdlib__
#define __stdlib__

#include <types.h>

extern char *itoa(int value, char *str, int base);
extern void itoa64(uint64_t value, char *buffer, int base);

#endif