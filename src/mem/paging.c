#include <mem/paging.h>
#include <stdio.h>

extern uint64_t p4_table[];    // PML4
extern uint64_t p3_table[];    // PDPR
extern uint64_t p3_table_hh[]; // PD
extern uint64_t p2_table[];    // PT

extern uint64_t pt_tables[];
extern uint64_t fbb_pt_tables[];

uint64_t read_cr3()
{
  uint64_t cr3;
  asm volatile("mov %%cr3, %0" : "=r"(cr3));
  return cr3;
}

uint64_t align_to_page(uint64_t address)
{
  return address & PAGE_MASK;
}

void print_page(uint64_t *pagedir, uint64_t index, uint8_t type, uint64_t offset)
{
  printf("%i: 0x%x: ", index, (void *)(&pagedir) - offset);

  // pml4 and pdpt
  if (type == 0 || type == 1)
  {
    struct PML4_entry *entry = (struct PML4_entry *)&pagedir;
    printf("p: %i, rw: %i, u: %i, pwt: %i, pcd: %i, acc: %i, ps: %i, addr: 0x%x, nx: %i",
           entry->present,
           entry->rw,
           entry->user_supervisor,
           entry->pwt,
           entry->pcd,
           entry->accessed,
           entry->ps,
           entry->pdt_addr << 12,
           entry->nx);
  }

  // pd and pt
  if (type == 2 || type == 3)
  {
    struct Pagetable_entry *entry = (struct Pagetable_entry *)&pagedir;
    printf("p: %i, rw: %i, u: %i, pwt: %i, pcd: %i, acc: %i, d: %i, p %i, g %i, addr: 0x%x, nx: %i",
           entry->present,
           entry->rw,
           entry->user_supervisor,
           entry->pwt,
           entry->pcd,
           entry->accessed,
           entry->dirty,
           entry->pat,
           entry->global,
           entry->pdt_addr << 12,
           entry->nx);
  }

  printf("\n");
}

void print_pagedir(uint64_t *pagedir, const char *name, uint32_t start, uint32_t count, uint8_t type, uint64_t offset)
{
  printf("%s\n", name);
  for (uint64_t i = start; i < start + count; i++)
  {
    if (pagedir[i] != 0)
    {
      print_page(pagedir[i], i, type, offset);
    }
  }
  printf("\n");
}

uint64_t is_physical_memory_mapped(uint64_t physical_address, uint64_t *pml4_table)
{
  uint64_t pml4_index = (physical_address >> 39) & ADDRESS_MASK;
  uint64_t pdpt_index = (physical_address >> 30) & ADDRESS_MASK;
  uint64_t pd_index = (physical_address >> 21) & ADDRESS_MASK;
  uint64_t pt_index = (physical_address >> 12) & ADDRESS_MASK;

  uint64_t pdpt_entry = pml4_table[pml4_index];
  if (!(pdpt_entry & PRESENT_BIT))
  {
    return 0;
  }

  uint64_t *pdpt_table = (uint64_t *)(pdpt_entry & PAGE_MASK);
  uint64_t pd_entry = pdpt_table[pdpt_index];
  if (!(pd_entry & PRESENT_BIT))
  {
    return 0;
  }

  uint64_t *pd_table = (uint64_t *)(pd_entry & PAGE_MASK);
  uint64_t pt_entry = pd_table[pd_index];
  if (!(pt_entry & PRESENT_BIT))
  {
    return 0;
  }

  uint64_t *pt_table = (uint64_t *)(pt_entry & PAGE_MASK);
  uint64_t page_entry = pt_table[pt_index];
  if (!(page_entry & 1))
  {
    return 0;
  }

  if ((page_entry & PAGE_MASK) == (physical_address & PAGE_MASK))
  {
    return 1;
  }

  return 0;
}

void setup_paging()
{
  printf("CR3: %x\n", read_cr3());
  uint64_t test_virtual_address = 0x40000000;
  uint64_t test_physical_address = 0x7FF0000; // 128MB
  map_page(test_virtual_address, test_physical_address, p4_table, PRESENT_BIT | RW_BIT);

  *(uint64_t *)test_virtual_address = 0xDEADBEEF;

  uint64_t value = *(uint64_t *)test_virtual_address;
  if (value == 0xDEADBEEF)
    printf("Woop woop \\o/\n");
  else
    printf(":(\n");

  // and then translate addresses
  uint64_t paddr = get_physical_address(test_virtual_address);
  printf("paddr: %p\n", paddr);
}

uint64_t *get_next_table_entry(uint64_t *current_table, uint64_t index)
{
  if (current_table == NULL || !(current_table[index] & PRESENT_BIT))
  {
    return NULL; // Not a valid table
  }
  return (uint64_t *)(current_table[index] & PAGE_MASK);
}

void *get_physical_address(uint64_t virtual_address)
{
  uint64_t pml4_index = (virtual_address >> 39) & ADDRESS_MASK;
  uint64_t pdpt_index = (virtual_address >> 30) & ADDRESS_MASK;
  uint64_t pd_index = (virtual_address >> 21) & ADDRESS_MASK;
  uint64_t pt_index = (virtual_address >> 12) & ADDRESS_MASK;
  uint64_t offset = virtual_address & ~PAGE_MASK;

  uint64_t *pml4_table = (uint64_t *)read_cr3();
  uint64_t *pdpt_table = get_next_table_entry(pml4_table, pml4_index);
  uint64_t *pd_table = get_next_table_entry(pdpt_table, pdpt_index);
  uint64_t *pt_table = get_next_table_entry(pd_table, pd_index);

  uint64_t physical_address = (pt_table[pt_index] & PAGE_MASK) | offset;
  return (void *)physical_address;
}

void setup_next_page_table(uint64_t *table_entry, uint64_t **next_entry, uint8_t flags)
{
  if (!(*table_entry & PRESENT_BIT))
  {
    uint64_t paddr = allocate_physical_page();
    *table_entry = paddr | flags;
  }

  *next_entry = (uint64_t *)(*table_entry & PAGE_MASK);
}

void map_page(uint64_t virtual_address, uint64_t physical_address, uint64_t *pml4_table, uint8_t flags)
{
  virtual_address = align_to_page(virtual_address);
  physical_address = align_to_page(physical_address);

  uint64_t pml4_index = (virtual_address >> 39) & ADDRESS_MASK;
  uint64_t pdpt_index = (virtual_address >> 30) & ADDRESS_MASK;
  uint64_t pd_index = (virtual_address >> 21) & ADDRESS_MASK;
  uint64_t pt_index = (virtual_address >> 12) & ADDRESS_MASK;

  uint64_t *pdpt_table = NULL;
  uint64_t *pd_table = NULL;
  uint64_t *pt_table = NULL;

  setup_next_page_table(&pml4_table[pml4_index], &pdpt_table, flags);
  setup_next_page_table(&pdpt_table[pdpt_index], &pd_table, flags);
  setup_next_page_table(&pd_table[pd_index], &pt_table, flags);

  pt_table[pt_index] = physical_address | flags;
}
