#include <string.h>
#include <mem/kheap.h>
#include <stdio.h>

int strlen(const char *str)
{
  int i = 0;
  while (str[i] != '\0')
    i++;
  return i;
}

char *strcpy(char *destination, const char *source)
{
  int len = strlen(source);
  for (int i = 0; i < len; i++)
  {
    destination[i] = source[i];
  }
  destination[len] = 0;
  return destination;
}

void *memcpy(void *destination, const void *source, size_t num)
{
  // yep, its not optimized at all :-D
  uint8_t *d = (uint8_t *)destination;
  const uint8_t *s = (uint8_t *)source;
  uint64_t len = num;

  for (size_t i = 0; i < len; i++)
  {
    d[i] = s[i];
  }

  return destination;
}

void *memmove(void *destination, const void *source, size_t num)
{
  // yep, its not optimized at all :-D
  uint8_t *d = (uint8_t *)destination;
  const uint8_t *s = (uint8_t *)source;
  uint64_t len = num;

  uint8_t *temp = kmalloc(num);

  for (size_t i = 0; i < len; i++)
  {
    temp[i] = s[i];
  }

  for (size_t i = 0; i < len; i++)
  {
    d[i] = temp[i];
  }

  kfree(temp);
  temp = NULL;

  return destination;
}

void *memset(void *ptr, int value, size_t num)
{
  uint8_t *destination = (uint8_t *)ptr;
  uint64_t len = num;

  for (uint64_t i = 0; i < len; i++)
  {
    destination[i] = value;
  }

  return ptr;
}