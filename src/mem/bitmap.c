#include <mem/bitmap.h>
#include <stdio.h>
#include "config.h"

extern uint64_t _kernel_physical_end;
extern uint64_t _kernel_end;

uint8_t *bitmap = NULL;

uint64_t total_pages = 0;

uint64_t *init_bitmap(uint64_t size)
{
  size = size / 4096 / BITS_PER_ROW;

#ifdef DEBUG
  printfs("Bitmap: %u bytes\n", size);
#endif

  bitmap = (uint8_t *)&_kernel_end;

  for (int i = 0; i < size; i++)
  {
    bitmap[i] = 0;
  }

  uint64_t amount_of_pages = (uint64_t)(&_kernel_physical_end) / PAGE_SIZE;
  printfs("kernel end: %u\n", amount_of_pages);
  total_pages = size * BITS_PER_ROW;

  set_page_range_used(0, (uint64_t)(&_kernel_physical_end) + (1024 * 1024 * 12));

  return bitmap;
}

int is_page_free(uint64_t page_index)
{
  return !(bitmap[page_index / BITS_PER_ROW] & (1 << (page_index % BITS_PER_ROW)));
}

void set_page_used(uint64_t page_index)
{
#ifdef DEBUG
  printfs("page index: %u / %u\n", page_index, total_pages);
#endif

  if (page_index >= total_pages)
  {
    printfs("bitmap error! page index: %u / %u: %i %s\n", page_index, total_pages, __LINE__, __FILE__);
    return;
  }

  int y = page_index / BITS_PER_ROW;
  bitmap[y] |= (1 << (page_index % BITS_PER_ROW));
}

void set_page_range_used(uint64_t memory_range_start, uint64_t memory_range_end)
{
  uint64_t page_start = memory_range_start / PAGE_SIZE;
  uint64_t page_end = memory_range_end / PAGE_SIZE;

#ifdef DEBUG
  printfs("bitmap range: %x:%x = %u:%u\n", memory_range_start, memory_range_end, page_start, page_end);
#endif

  for (uint64_t i = page_start; i < page_end; i++)
  {
    set_page_used(i);
  }
}

uint64_t allocate_physical_page()
{
  uint64_t return_addr = 0;
  for (uint64_t i = 0; i < total_pages; i++)
  {
    if (is_page_free(i))
    {
      set_page_used(i);
      return_addr = i * PAGE_SIZE;
      break;
    }
  }

#ifdef DEBUG
  printfs("allocate: %p (%i MB)\n", return_addr, return_addr / 1024 / 1024);
#endif
  return return_addr;
}
