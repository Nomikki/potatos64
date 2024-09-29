#include <drivers/serial.h>
#include <drivers/ports.h>

int serial_init()
{
  outportb(PORT_COM1 + 1, 0x00);    // Disable interrupts
  outportb(PORT_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  outportb(PORT_COM1 + 0, 0x03);    // Set divisor to 3 (low byte) 38400 baud
  outportb(PORT_COM1 + 1, 0x00);    // High byte
  outportb(PORT_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
  outportb(PORT_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  outportb(PORT_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set

  if (inportb(PORT_COM1 + 0) != 0xAE)
  {
    return 1;
  }

  outportb(PORT_COM1 + 4, 0x0F);

  return 0;
}

int is_transmit_empty() {
   return inportb(PORT_COM1 + 5) & 0x20;
}

void serial_write(char ch)
{
  while(is_transmit_empty() == 0);
  outportb(PORT_COM1, ch);
  
}

void serial_writeline(char * str)
{
  for (int i = 0; 1; i++)
  {
    char ch = str[i];
    if (ch == '\0')
    {
      serial_write('\r');
      serial_write('\n');
      return;
    }
    serial_write(ch);
  }
}