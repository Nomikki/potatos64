#include <mem/pmm.h>
#include "multiboot.h"
#include <types.h>
#include <stdio.h>
#include <string.h>

uint8_t *memory_bitmap;
size_t total_memory_size;
int64_t all_memory = 0;

struct multiboot_tag_mmap *tagmmap = NULL; // This tag provides memory map.
struct multiboot_tag_basic_meminfo *tagmem = NULL;

extern uint64_t multiboot_basic_meminfo;
extern uint64_t multiboot_mmap_data;

extern uint64_t _kernel_physical_end;
extern uint64_t _kern_virtual_offset;

void init_pmm()
{
  // find area for bitmap and return its address. If not enough memory/error, return NULL.
  uint64_t *bitmap_address = (void *)(init_bitmap(all_memory * 1024)) - (uint64_t)(&_kern_virtual_offset);
  printf("Bitmap address: %p\n", bitmap_address);

  /* From multiboot2 doc:
      ‘mem_lower’ and ‘mem_upper’ indicate the amount of lower and upper memory, respectively, in kilobytes.
      Lower memory starts at address 0, and upper memory starts at address 1 megabyte.
      The maximum possible value for lower memory is 640 kilobytes.
      The value returned for upper memory is maximally the address of the first upper memory hole minus 1 megabyte.
      It is not guaranteed to be this value.
    */
  tagmem = (struct multiboot_tag_basic_meminfo *)(multiboot_basic_meminfo + (uint64_t)(&_kern_virtual_offset));
  all_memory = tagmem->mem_lower + tagmem->mem_upper;

  /* From multiboot2 doc:
    The map provided is guaranteed to list all standard RAM that should be available for normal use.
    This type however includes the regions occupied by kernel, mbi, segments and modules.
    Kernel must take care not to overwrite these regions.
  */
  tagmmap = (struct multiboot_tat_mmap *)(multiboot_mmap_data + (uint64_t)(&_kern_virtual_offset));
  parse_mmap(tagmmap, all_memory * 1024);
  printf("Memory available: : (%u MB), (%u KB)\n", all_memory / 1024, all_memory);
  printf("Kernel physical end: %p\n", &_kernel_physical_end);
}
