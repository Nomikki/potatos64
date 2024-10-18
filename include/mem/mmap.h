#ifndef MEM_MMAP_H_
#define MEM_MMAP_H_

/* From multiboot2 doc:
    The (memopry)map provided is guaranteed to list all standard RAM that should be available for normal use.
    This type however includes the regions occupied by kernel, mbi, segments and modules.
    Kernel must take care not to overwrite these regions.
  */

#include "multiboot.h"
#include <types.h>

// parse memory maps
extern void parse_mmap(struct multiboot_tag_mmap *mmap_root, uint64_t size_of_memory);

#endif // MEM_MMAP_H_