#ifndef STDLIB_H_
#define STDLIB_H_

#include <types.h>

extern char *itoa(int value, char *str, int base);
extern void itoa64(uint64_t value, char *buffer, int base);

#endif // STDLIB_H_