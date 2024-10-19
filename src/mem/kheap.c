#include <mem/kheap.h>
#include <stdio.h>
#include <mem/vmm.h>

#include "config.h"

uint64_t *heap_head = 0;
uint64_t *heap_start = 0;

void heap_tests()
{
  printf("heap test:\n");

  uint64_t *ptr = NULL;
  uint64_t *ptr2 = NULL;
  ptr = kmalloc(32);
  ptr = kmalloc(32);

  ptr2 = ptr;
  ptr = kmalloc(32);
  ptr = kmalloc(32);

  kfree(ptr2);
  ptr = kmalloc(32);
  ptr = kmalloc(32);

  printf("travel through pointer map:\n");
  heap_node *current_node = (heap_node *)heap_start;
  while (current_node < heap_head)
  {

    printf("Allocated ptr %u: %u: %p\n", current_node->size, current_node->status, current_node);
    current_node += (current_node->size + sizeof(heap_node));
  }
  printf("heap test end\n");
}

void *kmalloc(size_t size)
{
  heap_node *current_node = (heap_node *)heap_start;
  while (current_node < heap_head)
  {
    // check if any memory holes big enough for allocations
    if (current_node->size >= size && current_node->status == HEAP_ALLOCATION_FREE)
    {
      current_node->status = HEAP_ALLOCATION_USED;
      return (uint64_t)current_node + sizeof(heap_node);
    }
    current_node += (current_node->size + sizeof(heap_node));
  }

  current_node->size = size;
  current_node->status = HEAP_ALLOCATION_USED;

  uint64_t address_to_return = (uint64_t)current_node + sizeof(heap_node);
  heap_head = address_to_return;
  return address_to_return;
}

void kfree(void *ptr)
{
  heap_node *node = (heap_node *)(ptr - sizeof(heap_node));
  if (node->status == HEAP_ALLOCATION_USED)
  {
    node->status = HEAP_ALLOCATION_FREE;
  }
  return;
}

void init_kheap()
{
  printf("Init KHEAP\n");
  // heap start = virtual address
  heap_head = vmm_allocate((1024 * 8), PT_PRESENT_BIT | PT_RW_BIT);
  heap_start = heap_head;
  // next = null
  // prev = null
  // status = free
  // init heap size (8k)
  // heap end = heap_start
  printf("KHEAP init OK\n\n");

  heap_tests();
}
