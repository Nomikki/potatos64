#ifndef MEM_MMAP_H_
#define MEM_MMAP_H_

#include "multiboot.h"

extern void mmap_parse(struct multiboot_tag_mmap *mmap_root);

#endif // MEM_MMAP_H_