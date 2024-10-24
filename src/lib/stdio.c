#include <stdio.h>
#include <drivers/vga.h>
#include "config.h"

int putchar(int ch)
{
  int x, y;
  vga_get_cursor_pos(&x, &y);
  vga_write_char(ch, x, y);

#ifdef DEBUG
  printfs("%c", ch);
#endif
  return ch;
}

int puts(const char *str)
{
  int i = 0;
  while (str[i] != '\0')
  {
    putchar(str[i]);
    i++;
  }

  if (i > 0)
  {
    putchar('\n');
    i++;
  }

  return i ? i : EOF;
}

void putstr(const char *str)
{
  int i = 0;
  while (str[i] != '\0')
  {
    putchar(str[i]);
    i++;
  }
  return;
}

void printf(const char *format, ...)
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
        uint32_t value = va_arg(args, uint32_t);
        char buffer[32];
        itoa64(value, buffer, 10);
        putstr(buffer);
        break;
      case 'i':
      case 'd':
      {
        int value = va_arg(args, int);
        char buffer[32];
        itoa64(value, buffer, 10);
        putstr(buffer);
        break;
      }
      case 'x':
      {
        int value = va_arg(args, int);
        char buffer[32];
        itoa64(value, buffer, 16);
        putstr(buffer);
        break;
      }
      case 'b':
      {
        int value = va_arg(args, int);
        char buffer[32];
        itoa(value, buffer, 2);
        putstr(buffer);
        break;
      }
      case 'B':
      {
        int value = va_arg(args, int);
        char buffer[65];
        itoa(value, buffer, 2);
        int len_of_value = strlen(buffer);
        for (int l = 0; l < 64 - len_of_value; l++)
        {
          putchar('0');
        }

        putstr(buffer);
        break;
      }
      case 'p':
      {
        void *ptr = va_arg(args, void *);
        uint64_t value = (uint64_t)ptr;
        char buffer[20];

        itoa64(value, buffer, 16);
        int len_of_value = strlen(buffer);

        putstr("0x");
        for (int l = 0; l < 16 - len_of_value; l++)
        {
          putchar('0');
        }

        putstr(buffer);
        break;
      }
      case 's':
      {
        char *str = va_arg(args, char *);
        putstr(str);
        break;
      }
      case 'c':
      {
        char ch = (char)va_arg(args, int);
        putchar(ch);
        break;
      }
      default:
      {
        putchar('%');
        putchar(format[i]);
        break;
      }
      }
    }
    else
    {
      putchar(format[i]);
    }
  }

  va_end(args);
}
