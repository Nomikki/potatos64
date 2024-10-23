#include <drivers/serial.h>
#include <drivers/ports.h>

int init_serial()
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

void write_serial(char ch)
{

  while (is_transmit_empty() == 0)
    ;
  outportb(PORT_COM1, ch);
}

void writeline_serial(char *str)
{
  for (int i = 0; 1; i++)
  {
    char ch = str[i];
    if (ch == '\0')
    {
      write_serial('\r');
      write_serial('\n');
      return;
    }
    write_serial(ch);
  }
}

void write_str_serial(char *str)
{

  for (int i = 0; 1; i++)
  {
    char ch = str[i];
    if (ch == '\0')
    {
      return;
    }
    write_serial(ch);
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
      case 'u':
        uint32_t value = va_arg(args, uint64_t);
        char buffer[32];
        itoa64(value, buffer, 10);
        write_str_serial(buffer);
        break;
      case 'i':
      case 'd':
      {
        int value = va_arg(args, int64_t);
        char buffer[32];
        itoa(value, buffer, 10);
        write_str_serial(buffer);
        break;
      }
      case 'x':
      {
        int value = va_arg(args, uint64_t);
        char buffer[32];
        itoa64(value, buffer, 16);
        write_str_serial(buffer);
        break;
      }
      case 'b':
      {
        int value = va_arg(args, uint64_t);
        char buffer[32];
        itoa(value, buffer, 2);
        write_str_serial(buffer);
        break;
      }
      case 'p':
      {

        void *ptr = va_arg(args, void *);
        uint64_t value = (uint64_t)ptr;
        char buffer[20];

        itoa64(value, buffer, 16);
        int len_of_value = strlen(buffer);

        write_str_serial("0x");
        for (int l = 0; l < 16 - len_of_value; l++)
        {
          write_serial('0');
        }

        write_str_serial(buffer);
        break;
      }
      case 's':
      {
        char *str = va_arg(args, char *);
        write_str_serial(str);
        break;
      }
      case 'c':
      {
        char ch = (char)va_arg(args, int);
        write_serial(ch);
        break;
      }
      default:
      {
        write_serial('%');
        write_serial(format[i]);
        break;
      }
      }
    }
    else
    {
      write_serial(format[i]);
    }
  }

  va_end(args);
}