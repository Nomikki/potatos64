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
  return address & ~0xfff;
}

void print_pagedir(uint64_t *pagedir, const char *name, uint32_t start, uint32_t count, uint8_t type, uint64_t offset)
{
  printf("%s\n", name);
  for (uint64_t i = start; i < start + count; i++)
  {
    if (pagedir[i] != 0)
    {
      printf("%i: 0x%x: ", i, (void *)(&pagedir[i]) - offset);

      if (type == 0 || type == 1)
      {
        struct PML4_Entry *entry = (struct PML4_Entry *)&pagedir[i];
        printf("p: %i, rw: %i, u: %i, pwt: %i, pcd: %i, acc: %i, ps: %i, addr: 0x%x, nx: %i", entry->present, entry->rw, entry->user_supervisor, entry->pwt, entry->pcd, entry->accessed, entry->ps, entry->pdt_addr << 12, entry->nx);
      }

      if (type == 2 || type == 3)
      {
        struct Pagetable_entry *entry = (struct Pagetable_entry *)&pagedir[i];
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
  }
  printf("\n");
}

void decompose_virtual_address(uint64_t virtual_address, uint64_t virtual_offset)
{
  uint64_t pml4_index = (virtual_address >> 39) & 0x1FF;
  uint64_t pdpr_index = (virtual_address >> 30) & 0x1FF;
  uint64_t pd_index = (virtual_address >> 21) & 0x1FF;
  uint64_t pt_index = (virtual_address >> 12) & 0xFFF;
  uint64_t offset = virtual_address & 0xfff;

  printf("Decomposing %p\n", virtual_address);
  printf("pml4 index: %u\n", pml4_index);
  printf("pdpr index: %u\n", pdpr_index);
  printf("pd index:   %u\n", pd_index);
  printf("pt index:   %u\n", pt_index);
  printf("offset      %u\n", offset);
  printf("\n");

  uint64_t pdt_address = (void *)(&p4_table[pml4_index]) - virtual_offset;
  printf("pdt adress: %p\n", pdt_address);

  uint64_t *pdt_table = (uint64_t *)(pdt_address & ~0xFFF); // remove lowest 12 bits
  uint64_t pd_address = (void *)(&pdt_table[pdpr_index]) - virtual_offset;
  printf("pd adress: %p\n", pd_address);

  uint64_t *pd_table = (uint64_t *)(pd_address & ~0xFFF);
  uint64_t pt_address = (void *)&pd_table[pd_index] - virtual_offset;
  printf("pt adress: %p\n", pt_address);
}

uint64_t is_physical_memory_mapped(uint64_t physical_address, uint64_t *pml4_table)
{
  uint64_t pml4_index = (physical_address >> 39) & 0x1FF;
  uint64_t pdpt_index = (physical_address >> 30) & 0x1FF;
  uint64_t pd_index = (physical_address >> 21) & 0x1FF;
  uint64_t pt_index = (physical_address >> 12) & 0x1FF;

  uint64_t pdpt_entry = pml4_table[pml4_index];
  if (!(pdpt_entry & 1))
  {
    return 0;
  }

  uint64_t *pdpt_table = (uint64_t *)(pdpt_entry & ~0xFFF);
  uint64_t pd_entry = pdpt_table[pdpt_index];
  if (!(pd_entry & 1))
  {
    return 0;
  }

  uint64_t *pd_table = (uint64_t *)(pd_entry & ~0xFFF);
  uint64_t pt_entry = pd_table[pd_index];
  if (!(pt_entry & 1))
  {
    return 0;
  }

  uint64_t *pt_table = (uint64_t *)(pt_entry & ~0xFFF);
  uint64_t page_entry = pt_table[pt_index];
  if (!(page_entry & 1))
  {
    return 0;
  }

  if ((page_entry & ~0xFFF) == (physical_address & ~0xFFF))
  {
    return 1;
  }

  return 0;
}

void setup_paging()
{
  printf("CR3: %x\n", read_cr3());
  uint64_t test_virtual_address = 0x40000000;
  uint64_t test_physical_address = align_to_page(0x7FF0000); // 128MB
  map_page(test_virtual_address, test_physical_address, p4_table);

  *(uint64_t *)test_virtual_address = 0xDEADBEEF;

  uint64_t value = *(uint64_t *)test_virtual_address;
  if (value == 0xDEADBEEF)
    printf("Woop woop \\o/\n");
  else
    printf(":(\n");
}

void map_page(uint64_t virtual_address, uint64_t physical_address, uint64_t *pml4_table)
{
  virtual_address = align_to_page(virtual_address);
  physical_address = align_to_page(physical_address);

  uint64_t pml4_index = (virtual_address >> 39) & 0x1FF;
  uint64_t pdpt_index = (virtual_address >> 30) & 0x1FF;
  uint64_t pd_index = (virtual_address >> 21) & 0x1FF;
  uint64_t pt_index = (virtual_address >> 12) & 0x1FF;

  if (!(pml4_table[pml4_index] & 1))
  {
    uint64_t pdpt = allocate_physical_page();
    pml4_table[pml4_index] = pdpt | 0x3;
  }
  uint64_t *pdpt_table = (uint64_t *)(pml4_table[pml4_index] & ~0xFFF);

  if (!(pdpt_table[pdpt_index] & 1))
  {
    uint64_t pd = allocate_physical_page();
    pdpt_table[pdpt_index] = pd | 0x3;
  }
  uint64_t *pd_table = (uint64_t *)(pdpt_table[pdpt_index] & ~0xFFF);

  if (!(pd_table[pd_index] & 1))
  {
    uint64_t pt = allocate_physical_page();
    pd_table[pd_index] = pt | 0x3;
  }
  uint64_t *pt_table = (uint64_t *)(pd_table[pd_index] & ~0xFFF);

  pt_table[pt_index] = physical_address | 0x3;
}
