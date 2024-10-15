#include <mem/bitmap.h>
#include <stdio.h>

extern uint64_t _kernel_physical_end;
extern uint64_t _kernel_end;

uint16_t *bitmap = NULL;

#define BITS_PER_ROW 16
#define PAGE_SIZE 4096

int total_pages = 0;

uint64_t *bitmap_init(uint32_t size)
{

  // debug
  printf("Bitmap: %u bytes\n", size);

  bitmap = (uint16_t *)&_kernel_end;

  for (int i = 0; i < size; i++)
  {
    bitmap[i] = 0;
  }

  uint64_t amount_of_pages = (uint64_t)(&_kernel_physical_end) / PAGE_SIZE;
  printf("kernel end: %u\n", amount_of_pages);

  set_page_range_used(0, (uint64_t)(&_kernel_physical_end) + 0x1000);

  total_pages = size * BITS_PER_ROW;

  return bitmap;
}

int is_page_free(uint64_t page_index)
{
  return !(bitmap[page_index / BITS_PER_ROW] & (1 << (page_index % BITS_PER_ROW)));
}

void set_page_used(uint64_t page_index)
{
  bitmap[page_index / BITS_PER_ROW] |= (1 << (page_index % BITS_PER_ROW));
}

void set_page_range_used(uint64_t memory_range_start, uint64_t memory_range_end)
{
  uint64_t page_start = memory_range_start / PAGE_SIZE;
  uint64_t page_end = memory_range_end / PAGE_SIZE;

  // debug
  // printf("bitmap range: %x:%x = %u:%u\n", memory_range_start, memory_range_end, page_start, page_end);

  for (int i = page_start; i < page_end; i++)
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
  // debug
  // printf("allocate: %x\n", return_addr);
  return return_addr;
}
