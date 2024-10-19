#include <utils/hexdump.h>
#include <stdio.h>

void hexdump(const void *start, const void *end)
{
  const uint8_t *ptr = (const uint8_t *)start;
  const uint8_t *end_ptr = (const uint8_t *)end;
  int skipped = 0;
  while (ptr < end_ptr)
  {
    int skip = 1;
    for (int i = 0; i < 16; i++)
    {
      if (ptr[i] != 0)
        skip = 0;
    }
    if (skip == 1)
    {
      ptr += 16;
      skipped = 1;
      continue;
    }
    if (skipped == 1)
    {
      skipped = 0;
      printf(" ... [SKIP] ...\n");
    }
    printf("%p: ", ptr);

    for (int i = 0; i < 16; i++)
    {
      if (ptr + i < end_ptr)
      {
        if (ptr[i] >= 10)
          printf("%x ", ptr[i]);
        else
          printf("0%x ", ptr[i]);
      }
      else
      {
        printf("   ");
      }
    }

    printf(" | ");
    for (int i = 0; i < 16; i++)
    {
      if (ptr + i < end_ptr)
      {
        char ch = ptr[i];
        printf("%c", (ch >= 32 && ch < 127) ? ch : '.');
      }
      else
      {
        printf(" ");
      }
    }

    printf("\n");
    ptr += 16;
  }
}
