#include <mem/kheap.h>
#include <stdio.h>
#include <mem/vmm.h>

#include "config.h"

heap_root *heap = NULL;

void kheap_print_travel()
{
  printf("\n");
  printf("travel through pointer map:\n");
  heap_node *current_node = (heap_node *)heap->heap_start;
  while (current_node < heap->heap_head)
  {
    uint64_t addr = (uint64_t)current_node; // - sizeof(heap_node);
    printf("Allocated ptr %u: %u: %p\n", current_node->size, current_node->status, addr);

    uint64_t next_offset = (uint64_t)(current_node->size + sizeof(heap_node));
    uint64_t next_address = (uint64_t)(current_node) + next_offset;
    current_node = next_address;
  }
  printf("--------------------\n");
}

void heap_tests()
{
  printf("heap test:\n");
  printf("Address of heap: %p\n", heap);

  uint64_t *ptr = NULL;
  uint64_t *ptr2 = NULL;
  uint64_t *ptr3 = NULL;

  ptr = (uint64_t *)kmalloc(10);
  printf("ptr: %p\n", ptr);

  ptr = (uint64_t *)kmalloc(10);
  printf("ptr: %p\n", ptr);
  ptr2 = ptr;

  ptr = (uint64_t *)kmalloc(20);
  ptr3 = ptr;
  printf("ptr: %p\n", ptr);

  ptr = (uint64_t *)kmalloc(10);
  printf("ptr: %p\n", ptr);

  ptr = (uint64_t *)kmalloc(10);

  kheap_print_travel();

  kfree(ptr3);

  kheap_print_travel();

  kfree(ptr2);

  for (int i = 0; i < 1024; i++)
  {
    ptr = (uint64_t *)kmalloc(1024 * 4);
    printf("ptr: %p\n", ptr);
  }

  // ptr = kmalloc(10);
  // printf("ptr: %p\n", ptr);
  kheap_print_travel();

  printf("heap test end\n");
}

void split(uint64_t starting_address, uint64_t size, uint64_t leftover)
{
  heap_node *A = (heap_node *)starting_address;
  uint64_t splitting_address = starting_address + size + sizeof(heap_node);
  heap_node *B = (heap_node *)(splitting_address);

  uint64_t original_size = A->size;

  A->size = size;
  A->status = HEAP_ALLOCATION_USED;

  B->size = leftover;
  B->status = HEAP_ALLOCATION_FREE;
}

void *kmalloc(size_t size)
{
#ifdef DEBUG
  printf("Allocate: %u bytes (0x%x) + header (%u bytes)\n", size, size, sizeof(heap_node));
#endif
  heap_node *current_node = (heap_node *)heap->heap_start;
  // printf("malloc start: %p\n", current_node);

  while (current_node < heap->heap_head)
  {

    // check if any memory holes big enough for allocations
    if (current_node->size >= size && current_node->status == HEAP_ALLOCATION_FREE)
    {
      current_node->status = HEAP_ALLOCATION_USED;

      // check if there is any room for split?
      uint64_t size_we_need = size + sizeof(heap_node);
      uint64_t size_of_gap = current_node->size;

      uint64_t leftover_size = size_of_gap - size_we_need;
#ifdef DEBUG
      printf("size we need: %u, size of hole (hihi): %u\n", size_we_need, size_of_gap);
      printf("Lefover: %u\n", leftover_size);
      printf("Original size: %u\n", current_node->size);
#endif

      uint64_t return_address = (uint64_t)(current_node) + sizeof(heap_node);

      if (size_we_need < size_of_gap)
      {
        split(current_node, size, leftover_size);
      }

      map_if_not_mapped(return_address);

      return return_address;
    }

    uint64_t next_offset = (uint64_t)(current_node->size + sizeof(heap_node));
    // printf("Offset: %u\n", next_offset);
    uint64_t next_address = (uint64_t)(current_node) + next_offset;

    map_if_not_mapped(next_address);

    current_node = next_address;
  }

  current_node->size = size;
  current_node->status = HEAP_ALLOCATION_USED;

  uint64_t return_address = (uint64_t)(current_node) + sizeof(heap_node);

  heap->heap_head = return_address;

  map_if_not_mapped(return_address);

  return return_address;
}

void kheap_merge(heap_node *current_node, heap_node *next, heap_node *previous)
{
  // merge
  if (next != NULL)
  {
    if (next->status == HEAP_ALLOCATION_FREE)
    {
      // all free, so we can merge them together!
      current_node->size += next->size + sizeof(heap_node);
    }
  }

  if (previous != NULL)
  {
    if (previous->status == HEAP_ALLOCATION_FREE)
    {
      // all free, so we can merge them together!
      previous->size += current_node->size + sizeof(heap_node);
    }
  }
}

void kfree(void *ptr)
{
  uint64_t addr = (uint64_t)(ptr) - sizeof(heap_node);
  heap_node *node = (heap_node *)addr;
#ifdef DEBUG
  printf("Try free memory at %p: \n", addr);
#endif

  heap_node *current_node = (heap_node *)heap->heap_start;
  heap_node *previous = NULL;
  heap_node *next = NULL;

  while (current_node < heap->heap_head)
  {
    next = (uint64_t)(current_node) + current_node->size + sizeof(heap_node);
    //  check if any memory holes big enough for allocations
    if (current_node == node && current_node->status == HEAP_ALLOCATION_USED)
    {
      current_node->status = HEAP_ALLOCATION_FREE;

      kheap_merge(current_node, next, previous);
      return;
    }

    uint64_t next_offset = (uint64_t)(current_node->size + sizeof(heap_node));
    uint64_t next_address = (uint64_t)(current_node) + next_offset;
    previous = current_node;
    current_node = next_address;
  }

  printf("KFREE ERROR! Can't free memory at %p. Node not found.\n", ptr);

  return;
}

void init_kheap()
{
  printf("Init KHEAP\n");
  // allocate some space for heap
  heap = vmm_allocate((1024 * 8), PT_PRESENT_BIT | PT_RW_BIT);
  heap->heap_head = heap + sizeof(heap_node);
  heap->heap_start = heap->heap_head;

  printf("KHEAP init OK\n\n");

  heap_tests();
}
