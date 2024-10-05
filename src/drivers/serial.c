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

void serial_write_str(char *str)
{

  for (int i = 0; 1; i++)
  {
    char ch = str[i];
    if (ch == '\0')
    {
      return;
    }
    serial_write(ch);
  }
}

void printfs(const char *format, ...)
{
  va_list args;
  va_start(args, format);

  for (int i = 0; format[i] != '\0'; i++)
  {
    if (format[i] == '%')
    {
      i++;
      switch (format[i])
      {
      case 'i':
      case 'd':
      {
        int value = va_arg(args, int);
        char buffer[32];
        itoa(value, buffer, 10);
        serial_write_str(buffer);
        break;
      }
      case 'x':
      {
        int value = va_arg(args, int);
        char buffer[32];
        itoa(value, buffer, 16);
        serial_write_str(buffer);
        break;
      }
      case 'b':
      {
        int value = va_arg(args, int);
        char buffer[32];
        itoa(value, buffer, 2);
        serial_write_str(buffer);
        break;
      }
      case 'p':
      {
        void *ptr = va_arg(args, void *);
        uint64_t value = (uint64_t)ptr;
        char buffer[20];

        itoa64(value, buffer, 16);
        int len_of_value = strlen(buffer);

        serial_write_str("0x");
        for (int l = 0; l < 16 - len_of_value; l++)
        {
          serial_write('0');
        }

        serial_write_str(buffer);
        break;
      }
      case 's':
      {
        char *str = va_arg(args, char *);
        serial_write_str(str);
        break;
      }
      case 'c':
      {
        char ch = (char)va_arg(args, int);
        serial_write(ch);
        break;
      }
      default:
      {
        serial_write('%');
        serial_write(format[i]);
        break;
      }
      }
    }
    else
    {
      serial_write(format[i]);
    }
  }

  va_end(args);
}