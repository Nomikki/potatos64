#include <mem/kheap.h>
#include <stdio.h>
#include <mem/vmm.h>

#include "config.h"

uint64_t *cur = 0;
uint64_t *start_ptr = 0;
#define USED 1
#define FREE 0

void heap_tests()
{
  printf("heap test:\n");

  uint64_t *ptr = NULL;
  uint64_t *ptr2 = NULL;
  ptr = kmalloc(32);
  printf("address of ptr: %p\n", ptr);
  ptr = kmalloc(32);
  ptr = kmalloc(32);
  ptr = kmalloc(32);
  ptr2 = ptr;

  kfree(ptr2);
  ptr = kmalloc(32);
  ptr = kmalloc(32);

  printf("travel through pointer map:\n");
  uint64_t *travel_ptr = start_ptr;
  while (travel_ptr < cur)
  {
    printf("Allocated ptr %u: %u: %p\n", *travel_ptr, *(travel_ptr + 1), travel_ptr + 2);
    if (*travel_ptr == 0)
      break;
    travel_ptr += (*travel_ptr / sizeof(uint64_t)) + 2;
  }
  printf("heap test end\n");
}

void init_kheap()
{
  printf("Init KHEAP\n");
  // heap start = virtual address
  cur = vmm_allocate((1024 * 8), PT_PRESENT_BIT | PT_RW_BIT);
  start_ptr = cur;
  // next = null
  // prev = null
  // status = free
  // init heap size (8k)
  // heap end = heap_start
  printf("KHEAP init OK\n\n");

  heap_tests();
}

void *kmalloc(size_t size)
{
  // is cur position allocated? if not, allocate it!

  uint64_t *travel_ptr = start_ptr;
  while (travel_ptr < cur)
  {
    uint64_t cur_size = *travel_ptr;
    uint64_t cur_status = *(travel_ptr + 1);

    if (cur_status == FREE && cur_size >= size)
    {

      printf("free found: %u: %u: %p\n", *travel_ptr, *(travel_ptr + 1), travel_ptr + 2);
      *(travel_ptr + 1) = USED;
      return travel_ptr + 2;
    }

    travel_ptr += (*travel_ptr / sizeof(uint64_t)) + 2;
  }

  *cur = size;
  cur = cur + 1;

  *cur = USED;
  cur = cur + 1;

  uint64_t address_to_return = cur;
  cur += (size / sizeof(uint64_t));
  return address_to_return;
}

void kfree(void *ptr)
{
  uint64_t *test_ptr = (uint64_t *)ptr;

  if (*(test_ptr - 1) == USED)
  {
    *(test_ptr - 1) = FREE;
  }
  return;
}