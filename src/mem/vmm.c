#include <mem/vmm.h>
#include <stdio.h>
#include <mem/paging.h>
#include <mem/bitmap.h>
#include "config.h"

extern uint64_t p4_table[]; // PML4
extern uint64_t _kernel_end;
extern uint64_t _kernel_start;
extern uint64_t _kern_virtual_offset;
extern uint64_t end_of_mapped_memory;
extern uint64_t vmm_area;

vmm_region *free_list = NULL;

uint64_t convert_x86_64_vm_flags(size_t flags)
{
  uint64_t value = 0;
  if (flags & VM_FLAG_WRITE)
    value |= PT_RW_BIT;
  if (flags & VM_FLAG_EXEC)
    value |= PT_USER_BIT;
  if ((flags & VM_FLAG_USER) == 0)
    value |= PT_NX_BIT;

  return value;
}

void vmm_add_region(uint64_t start, uint64_t size)
{
  // vmm_region *new_region = (vmm_region *)allocate_physical_page
}

void vmm_free(void *addr)
{
}

uint64_t vmm_allocate(uint64_t size, uint8_t flags)
{

  // align size to page boundary
  size = align_to_page((size));

  uint64_t starting_virtual_address = 0;

  // printf("Allocate: %i bytes (%i KB) with 0b%b flags\n", size, size / 1024, flags);

  // vmm_region *prev = NULL;
  // vmm_region *cur = (vmm_region *)free_list;

  // how many pages we need?
  // printf("Pages needed: %i\n", size / PAGE_SIZE);
  uint64_t amount_of_pages = size / PAGE_SIZE;

  for (uint64_t i = 0; i < amount_of_pages; i++)
  {

    uint64_t phys_addr = allocate_physical_page();
    // printf("phys: %p, %u MB\n", (uint64_t)phys_addr, phys_addr / 1024 / 1024);
    if (phys_addr == 0)
    {
      // TODO:
      // need to free reserved physical pages
      printfs("ERR: %u, %s\n", __LINE__, __FILE__);
      return NULL;
    }

    if (starting_virtual_address == 0)
      starting_virtual_address = free_list->base;

    if (free_list->len > 0x1000)
    {
      uint64_t mapping_address = free_list->base;

#ifdef DEBUG
      printfs("mapping addr: %p, phys addr: %p\n", mapping_address, phys_addr);
#endif

      map_page(mapping_address, phys_addr, p4_table, flags);
      free_list->base += 0x1000;
      free_list->len -= 0x1000;
    }
    else
    {
      printfs("ERR: %u, %s\n", __LINE__, __FILE__);
      return NULL;
    }
  }

  return starting_virtual_address;
}

void init_vmm()
{
  printf("Init VMM\n");

  // make some room for linked list
  uint64_t vmm_start_physical_address = &vmm_area - &_kern_virtual_offset;
  printf("VMM area: %p: %p\n", &vmm_area, vmm_start_physical_address);

  set_page_used(vmm_start_physical_address / 0x1000);
  map_page(vmm_area, vmm_start_physical_address, p4_table, PT_PRESENT_BIT | PT_RW_BIT);

  free_list = (vmm_region *)&vmm_area;
  free_list->base = 0xFFFFFFFFFF000000; //(uint64_t)((&end_of_mapped_memory) + (0x1000)) & ~0xFFF;
  free_list->len = 1024 * 1024 * 128;

  uint64_t *ptr = NULL;
  /*
  for (int i = 0; i < 128; i++)
  {
    ptr = vmm_allocate(1024 * 1024, PT_PRESENT_BIT | PT_RW_BIT);
  }
  */

  printf("vmm init ok\n");
}