#include <mem/bitmap.h>
#include <stdio.h>

// extern uint64_t _kernel_physical_end;
extern uint64_t _kernel_end;

uint16_t *bitmap = NULL;

#define BITS_PER_ROW 16
#define PAGE_SIZE 4096

uint64_t *bitmap_init(uint32_t size)
{

  printf("Bitmap: %u bytes\n", size);
  bitmap = (uint16_t *)&_kernel_end;

  for (int i = 0; i < size; i++)
  {
    bitmap[i] = 0;
  }

  return bitmap;
}

void bitmap_set_page(uint64_t page_address, bool present)
{
  uint64_t bitmap_location = page_address / PAGE_SIZE;

  uint64_t row = bitmap_location / (BITS_PER_ROW);
  uint64_t column = bitmap_location % (BITS_PER_ROW);
  if (present)
  {
    bitmap[row] |= 1 << column;
  }
  else
  {
    bitmap[row] &= ~(1 << column);
  }
}

int bitmap_test(uint64_t address)
{
  uint64_t bitmap_location = address / PAGE_SIZE;
  uint64_t column = bitmap_location % BITS_PER_ROW;

  int val = (bitmap[bitmap_location / BITS_PER_ROW] >> column) & 1;
  // printf("test: column: %i, final: %b, val: %i\n", column, bitmap[bitmap_location], val);

  // uint64_t column = bitmap_location & BITS_PER_ROW;
  return val;
}