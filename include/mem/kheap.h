#ifndef MEM_KHEAP_H_
#define MEM_KHEAP_H_

#include <types.h>

extern void *kmalloc(size_t size);
extern void kfree(void *ptr);

#endif // MEM_KHEAP_H_