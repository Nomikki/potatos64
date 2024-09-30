#include <drivers/serial.h>
#include <drivers/ports.h>

int serial_init()
{
  outportb(PORT_COM1 + INTERRUPT_REGISTER, DISABLE_INTERRUPTS);                                                           // Disable interrupts
  outportb(PORT_COM1 + LINE_CONTROL_REGISTER, ENABLE_DLAB);                                                               // Enable DLAB (set baud rate divisor)
  outportb(PORT_COM1 + BAUDRATE_LEAST_SIGNIFANT, 0x03);                                                                   // Set divisor to 3 (low byte) 38400 baud
  outportb(PORT_COM1 + BAUDRATE_MOST_SIGNIFANT, 0x00);                                                                    // High byte
  outportb(PORT_COM1 + LINE_CONTROL_REGISTER, BITS_8 | NO_PARITY | STOP_BIT);                                             // 8 bits, no parity, one stop bit
  outportb(PORT_COM1 + FIFO_CONTROL_REGISTER, ENABLE_FIFO | INT_THRESHOLD_14 | CLEAR_RECEIVE_FIFO | CLEAR_TRANSMIT_FIFO); // Enable FIFO, clear them, with 14-byte threshold
  outportb(PORT_COM1 + MODEM_CONTROL_REGISTER, ENABLE_IRQ | REQUEST_TO_SEND | DATA_TERMINAL_READY);                       // IRQs enabled, RTS/DSR set

  return 0;
}

int is_transmit_empty()
{
  return inportb(PORT_COM1 + LINE_STATUS_REGISTER) & TRANSMITTER_HOLDING_REGISTER_EMPTY;
}

void serial_write(char ch)
{
  while (is_transmit_empty() == 0)
    ;
  outportb(PORT_COM1, ch);
}

void serial_writeline(char *str)
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