#ifndef MEM_PAGING_H_
#define MEM_PAGING_H_

#include <types.h>

typedef struct __attribute__((packed))
{
  uint64_t present : 1;         // page is present in memory
  uint64_t rw : 1;              // read/write flag (1 = writable, 0 = read only)
  uint64_t user_supervisor : 1; // user/supervisor /1 = user-level, 0 = supervisor)
  uint64_t pwt : 1;             // page write through
  uint64_t pcd : 1;             // page cache disable
  uint64_t accessed : 1;        // accessed/dirty flag
  uint64_t ignored1 : 1;
  uint64_t ps : 1; // page size (should be 0 for PML4)
  uint64_t ignored2 : 4;
  uint64_t pdt_addr : 40; // physical address of PDPT (page directory pointer table)
  uint64_t reserved : 11;
  uint64_t nx : 1; // no execute bit
} PML4_entry;

typedef struct __attribute__((packed))
{
  uint64_t present : 1;         // page is present in memory
  uint64_t rw : 1;              // read/write flag (1 = writable, 0 = read only)
  uint64_t user_supervisor : 1; // user/supervisor /1 = user-level, 0 = supervisor)
  uint64_t pwt : 1;             // page write through
  uint64_t pcd : 1;             // page cache disable
  uint64_t accessed : 1;        // accessed/dirty flag
  uint64_t dirty : 1;
  uint64_t pat : 1; // page attribute table index
  uint64_t global : 1;
  uint64_t ignored2 : 3;
  uint64_t pdt_addr : 40; // physical address of PDPT (page directory pointer table)
  uint64_t reserved : 11;
  uint64_t nx : 1; // no execute bit
} Pagetable_entry;

#define PRESENT_BIT 0b1
#define RW_BIT 0b10
#define PAGE_MASK ~0xFFF
#define ADDRESS_MASK 0x1FF

extern void setup_paging();
extern void map_page(uint64_t virtual_address, uint64_t physical_address, uint64_t *pml4_table, uint8_t flags);
extern uint64_t is_physical_memory_mapped(uint64_t physical_address, uint64_t *pml4_table);
extern uint64_t align_to_page(uint64_t address);
extern void *get_physical_address(uint64_t virtual_address);

#endif // MEM_PAGING_H_
