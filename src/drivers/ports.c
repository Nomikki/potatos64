#include <drivers/ports.h>

extern inline unsigned char inportb(int portnum)
{
  unsigned char data = 0;
  //__asm__ __volatile__ ("inb %%dx, %%al" : "=a" (data) : "d" (portnum));
  __asm__ volatile ("inb %1, %0" : "=a"(data) : "Nd"(portnum));

  return data;
}

extern inline void outportb(int portnum, unsigned char data)
{
  //__asm__ __volatile__ ("outb %%al, %%dx" :: "a" (data), "d" (portnum));
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(portnum));
}