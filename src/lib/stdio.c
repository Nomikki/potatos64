#include <stdio.h>
#include <drivers/vga.h>

int putchar(int ch)
{
  int x, y;
  vga_get_cursor_pos(&x, &y);
  vga_write_char(ch, x, y);
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
