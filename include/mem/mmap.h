#ifndef MEM_MMAP_H_
#define MEM_MMAP_H_

#include "multiboot.h"
#include <types.h>

extern void parse_mmap(struct multiboot_tag_mmap *mmap_root, uint64_t size_of_memory);

#endif // MEM_MMAP_H_