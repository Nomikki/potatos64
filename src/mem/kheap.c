#include <mem/kheap.h>
#include <stdio.h>
#include <mem/vmm.h>

#include "config.h"

heap_root *heap = NULL;

void heap_tests()
{
  printf("heap test:\n");
  printf("Address of heap: %p\n", heap);

  uint64_t *ptr = NULL;
  uint64_t *ptr2 = NULL;

  ptr = (uint64_t *)kmalloc(10);
  printf("ptr: %p\n", ptr);

  ptr = (uint64_t *)kmalloc(50);
  printf("ptr: %p\n", ptr);
  ptr2 = ptr;

  ptr = (uint64_t *)kmalloc(10);
  printf("ptr: %p\n", ptr);

  ptr = (uint64_t *)kmalloc(10);
  printf("ptr: %p\n", ptr);

  kfree(ptr2);

  ptr = kmalloc(10);
  printf("ptr: %p\n", ptr);

  printf("travel through pointer map:\n");
  heap_node *current_node = (heap_node *)heap->heap_start;
  while (current_node < heap->heap_head)
  {
    uint64_t addr = (uint64_t)current_node - sizeof(heap_node);
    printf("Allocated ptr %u: %u: %p\n", current_node->size, current_node->status, addr);

    uint64_t next_offset = (uint64_t)(current_node->size + sizeof(heap_node));
    uint64_t next_address = (uint64_t)(current_node) + next_offset;
    current_node = next_address;
  }
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

      split(current_node, size, leftover_size);

      return return_address;
    }

    uint64_t next_offset = (uint64_t)(current_node->size + sizeof(heap_node));
    // printf("Offset: %u\n", next_offset);
    uint64_t next_address = (uint64_t)(current_node) + next_offset;
    current_node = next_address;
  }

  current_node->size = size;
  current_node->status = HEAP_ALLOCATION_USED;

  uint64_t return_address = (uint64_t)(current_node) + sizeof(heap_node);

  heap->heap_head = return_address;
  return return_address;
}

void kfree(void *ptr)
{
  uint64_t addr = (uint64_t)(ptr) - sizeof(heap_node);
  heap_node *node = (heap_node *)addr;
#ifdef DEBUG
  printf("Try free memory at %p: ", addr);
#endif

  if (node->status == HEAP_ALLOCATION_USED)
  {
#ifdef DEBUG
    printf("success!\n");
#endif
    node->status = HEAP_ALLOCATION_FREE;
  }
  else
  {
#ifdef DEBUG
    printf("fail\n");
#endif
  }
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
