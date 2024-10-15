#ifndef DRIVERS_PORTS_H_
#define DRIVERS_PORTS_H_

extern inline unsigned char inportb(int portnum);
extern inline void outportb(int portnum, unsigned char data);

#endif // DRIVERS_PORTS_H_
