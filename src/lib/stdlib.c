#include <stdlib.h>
#include <stdio.h>

void reverse(char s[])
{
  int i, j;
  char c;
  for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
  {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

char *itoa(int value, char *str, int base)
{
  int i = 0;
  int is_negative = 0;

  if (value < 0 && base == 10)
  {
    is_negative = 1;
    value = -value;
  }

  do
  {
    int rem = value % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
  } while (value /= base);

  if (is_negative)
  {
    str[i++] = '-';
  }

  str[i] = '\0';

  reverse(str);
  return str;
}

void itoa64(uint64_t value, char *buffer, int base)
{
  char *digits = "0123456789ABCDEF";
  char temp[65];
  int i = 0;

  if (value == 0)
  {
    buffer[0] = '0';
    buffer[1] = '\0';
    return;
  }

  while (value != 0)
  {
    temp[i++] = digits[value % base];
    value /= base;
  }

  int j = 0;
  while (i > 0)
  {
    buffer[j++] = temp[--i];
  }
  buffer[j] = '\0';
}
