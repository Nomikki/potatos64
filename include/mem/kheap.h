#ifndef MEM_KHEAP_H_
#define MEM_KHEAP_H_

#include <types.h>

typedef struct __attribute__((packed))
{
  uint64_t size;
  uint8_t status;
} heap_node;

#define HEAP_ALLOCATION_USED 1
#define HEAP_ALLOCATION_FREE 0

extern void init_kheap();
extern void *kmalloc(size_t size);
extern void kfree(void *ptr);

#endif // MEM_KHEAP_H_