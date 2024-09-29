#include <drivers/ports.h>

extern inline unsigned char inportb(int portnum)
{
  unsigned char data = 0;
  __asm__ volatile ("inb %1, %0" : "=a"(data) : "Nd"(portnum));
  return data;
}

extern inline void outportb(int portnum, unsigned char data)
{
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(portnum));
}