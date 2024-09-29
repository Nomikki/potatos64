#ifndef __drivers__ports__
#define __drivers__ports__

extern inline unsigned char inportb(int portnum);
extern inline void outportb(int portnum, unsigned char data);

#endif