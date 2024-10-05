#ifndef __stdio__
#define __stdio__

#include <stdarg.h>
#include <types.h>
#include <stdlib.h>

#define NULL 0
#define EOF (-1)

extern int puts(const char *str);
extern int putchar(int ch);
extern void printf(const char *format, ...);


#endif