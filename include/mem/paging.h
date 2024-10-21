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

#define PT_PRESENT_BIT (1 << 0)
#define PT_RW_BIT (1 << 1)
#define PT_USER_BIT (1 << 2)

#define PT_NX_BIT (1 << 63)

#define PT_PAGE_MASK ~0xFFF
#define PT_ADDRESS_MASK 0x1FF

#define SIGN_EXTENSION 0xFFFF000000000000
#define ENTRIES_TO_ADDRESS(pml4, pdpr, pd, pt) ((pml4 << 39) | (pdpr << 30) | (pd << 21) | (pt << 12))

#define PML4_ENTRY(address) ((address >> 39) & 0x1ff)
#define PDPR_ENTRY(address) ((address >> 30) & 0x1ff)
#define PD_ENTRY(address) ((address >> 21) & 0x1ff)
#define PT_ENTRY(address) ((address >> 12) & 0x1ff)

extern void setup_paging();
extern void map_page(uint64_t virtual_address, uint64_t physical_address, uint64_t *pml4_table, uint8_t flags);
extern uint64_t is_physical_memory_mapped(uint64_t physical_address, uint64_t *pml4_table);
extern int is_virtual_memory_mapped(uint64_t virtual_address);
extern void map_if_not_mapped(uint64_t address);

extern uint64_t align_to_page(uint64_t address);
extern void *get_physical_address(uint64_t virtual_address);
extern void print_pages();

#endif // MEM_PAGING_H_
