#ifndef MEM_VMM_H_
#define MEM_VMM_H_

#include <types.h>
#include <mem/paging.h>

// linked list for virtual memory map
typedef struct
{
  uint64_t base;           // start address of region
  uint64_t len;            // size of region
  struct vmm_region *next; // next region in list
} vmm_region;

#define VM_FLAG_NONE 0
#define VM_FLAG_WRITE (1 << 0)
#define VM_FLAG_EXEC (1 << 1)
#define VM_FLAG_USER (1 << 2)

extern void init_vmm();

extern uint64_t convert_x86_64_vm_flags(size_t flags);
extern uint64_t vmm_allocate(uint64_t size, uint8_t flags);

#endif // MEM_VMM_H_