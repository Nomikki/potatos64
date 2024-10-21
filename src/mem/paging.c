#include <mem/paging.h>
#include <stdio.h>
#include <drivers/serial.h>

extern uint64_t p4_table[]; // PML4
extern uint64_t p3_table[]; // PDPR
// extern uint64_t p3_table_hh[]; // PD
extern uint64_t p2_table[]; // PT

extern uint64_t pt_tables[];
extern uint64_t fbb_pt_tables[];

extern uint64_t _kern_virtual_offset;

uint64_t read_cr3()
{
  uint64_t cr3;
  asm volatile("mov %%cr3, %0" : "=r"(cr3));
  return cr3;
}

uint64_t align_to_page(uint64_t address)
{
  return address & PT_PAGE_MASK;
}

void print_page(uint64_t **pagedir, uint64_t index, uint8_t type, uint64_t offset)
{
}

void print_pagedir(uint64_t *pagedir, const char *name, uint32_t start, uint32_t count, uint8_t type, uint64_t offset)
{
  printfs("%s\n", name);
  for (uint64_t index = start; index < start + count; index++)
  {
    if (pagedir[index] != 0)
    {
      // print_page(pagedir[i], i, type, offset);
      printfs("%i: %p: ", index, (void *)(&pagedir[index]) - offset);

      // pml4 and pdpt
      if (type == 0 || type == 1)
      {
        PML4_entry *entry = (PML4_entry *)&pagedir[index];
        printfs("p: %i, rw: %i, u: %i, pwt: %i, pcd: %i, acc: %i, ps: %i, addr: 0x%x, nx: %i",
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
        Pagetable_entry *entry = (Pagetable_entry *)&pagedir[index];
        printfs("p: %i, rw: %i, u: %i, pwt: %i, pcd: %i, acc: %i, d: %i, p %i, g %i, addr: 0x%x, nx: %i",
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

      printfs("\n");
    }
  }
  printfs("\n");
}

uint64_t is_physical_memory_mapped(uint64_t physical_address, uint64_t *pml4_table)
{
  uint64_t pml4_index = (physical_address >> 39) & PT_ADDRESS_MASK;
  uint64_t pdpt_index = (physical_address >> 30) & PT_ADDRESS_MASK;
  uint64_t pd_index = (physical_address >> 21) & PT_ADDRESS_MASK;
  uint64_t pt_index = (physical_address >> 12) & PT_ADDRESS_MASK;

  uint64_t pdpt_entry = pml4_table[pml4_index];
  if (!(pdpt_entry & PT_PRESENT_BIT))
  {
    return 0;
  }

  uint64_t *pdpt_table = (uint64_t *)(pdpt_entry & PT_PAGE_MASK);
  uint64_t pd_entry = pdpt_table[pdpt_index];
  if (!(pd_entry & PT_PRESENT_BIT))
  {
    return 0;
  }

  uint64_t *pd_table = (uint64_t *)(pd_entry & PT_PAGE_MASK);
  uint64_t pt_entry = pd_table[pd_index];
  if (!(pt_entry & PT_PRESENT_BIT))
  {
    return 0;
  }

  uint64_t *pt_table = (uint64_t *)(pt_entry & PT_PAGE_MASK);
  uint64_t page_entry = pt_table[pt_index];
  if (!(page_entry & 1))
  {
    return 0;
  }

  if ((page_entry & PT_PAGE_MASK) == (physical_address & PT_PAGE_MASK))
  {
    return 1;
  }

  return 0;
}

void print_pages()
{
  printfs("pml4: %p\n", (uint64_t)(&p4_table));
  printfs("pdpt: %p\n", (uint64_t)(&p3_table));
  printfs("pd: %p\n", (uint64_t)(&p2_table));
  printfs("pt: %p\n", (uint64_t)(&pt_tables));

  print_pagedir(p4_table, "PML  ", 0, 512, 0, 0);
  print_pagedir(p3_table, "PDPT  ", 0, 512, 1, 0);
  print_pagedir(p2_table, "PD  ", 0, 512, 2, 0);
  print_pagedir(pt_tables, "PT  ", 0, 512, 3, 0);

  printfs("...........\n");

  for (int i = 0; i < 512; i++)
  {
    if (p3_table[i] & PT_PRESENT_BIT)
    {
      printfs("PDPT Index %u is present: 0x%x\n", i, p3_table[i]);
    }
  }
  printfs("...........\n");
}

void setup_paging()
{
  printf("CR3: %x\n", read_cr3());

  // set_page_range_used(0x5D3000, 0x5D3000 * 0x20);

  /*
  uint64_t test_virtual_address = 0x40000000;
  uint64_t test_physical_address = 0x7FF0000; // 128MB
  map_page(test_virtual_address, test_physical_address, p4_table, PT_PRESENT_BIT | PT_RW_BIT);

  *(uint64_t *)test_virtual_address = 0xDEADBEEF;

  uint64_t value = *(uint64_t *)test_virtual_address;
  if (value == 0xDEADBEEF)
    printf("Woop woop \\o/\n");
  else
    printf(":(\n");

  // and then translate addresses
  uint64_t paddr = get_physical_address(test_virtual_address);
  printf("paddr: %p\n", paddr);
  */
  printf("Virtual offset: %p\n", &_kern_virtual_offset);

  print_pages();
}

uint64_t *get_next_table_entry(uint64_t *current_table, uint64_t index)
{
#ifdef DEBUG
  printfs("get netxt table [%i] %p\n", index, current_table);
#endif

  if (current_table == NULL || !(current_table[index] & PT_PRESENT_BIT))
  {
    printfs("table [%i] %p is not valid.\n", index, current_table);
    return NULL; // Not a valid table
  }
  return (uint64_t *)(current_table[index] & PT_PAGE_MASK);
}

uint64_t translate_virtual_address(uint64_t virtual_address)
{

  uint64_t pml4_index = (virtual_address >> 39) & 0x1FF;
  uint64_t pdpt_index = (virtual_address >> 30) & 0x1FF;
  uint64_t pd_index = (virtual_address >> 21) & 0x1FF;
  uint64_t pt_index = (virtual_address >> 12) & 0x1FF;
  uint64_t offset = virtual_address & 0xFFF;

#ifdef DEBUG
  printfs("PML4 Index: %u, PDPT Index: %u, PD Index: %u, PT Index: %u\n",
          pml4_index, pdpt_index, pd_index, pt_index);
#endif

  uint64_t *pml4_table = (uint64_t *)0x109000;
  uint64_t pdpt_address = pml4_table[pml4_index];

  if (!(pdpt_address & 1))
  {
    printfs("PDPT invalid\n");
    return 0;
  }

  printfs("PDPT Address: %p\n", (void *)pdpt_address);

  uint64_t *pdpt_table = (uint64_t *)(pdpt_address & ~0xFFF);
  uint64_t pd_address = pdpt_table[pdpt_index];

  if (!(pd_address & 1))
  {
    printfs("PD invalid\n");
    return 0;
  }

  printfs("PD Address: %p\n", (void *)pd_address);

  uint64_t *pd_table = (uint64_t *)(pd_address & ~0xFFF);
  uint64_t pt_address = pd_table[pd_index];

  if (!(pt_address & 1))
  {
    printfs("PT invalid\n");
    return 0;
  }

  uint64_t *pt_table = (uint64_t *)(pt_address & ~0xFFF);
  uint64_t physical_address = (pt_table[pt_index] & ~0xFFF) | offset;

#ifdef DEBUG
  printfs("PT Address: %p\n", (void *)pt_address);
  printfs("Physical Address: %p\n", (void *)physical_address);
#endif

  return physical_address;
}

void *get_physical_address(uint64_t virtual_address)
{
  uint64_t pml4_index = (virtual_address >> 39) & PT_ADDRESS_MASK;
  uint64_t pdpt_index = (virtual_address >> 30) & PT_ADDRESS_MASK;
  uint64_t pd_index = (virtual_address >> 21) & PT_ADDRESS_MASK;
  uint64_t pt_index = (virtual_address >> 12) & PT_ADDRESS_MASK;
  uint64_t offset = virtual_address & ~PT_PAGE_MASK;

  uint64_t *pml4_table = p4_table;
  uint64_t *pdpt_table = get_next_table_entry(pml4_table, pml4_index);
  uint64_t *pd_table = get_next_table_entry(pdpt_table, pdpt_index);
  uint64_t *pt_table = get_next_table_entry(pd_table, pd_index);
#ifdef DEBUG
  printfs("get physical address from virtual address %p: \n", virtual_address);
  printfs("pml4: ");
  printfs("pdpt: ");
  printfs("pd: ");
#endif

  return (void *)((pt_table[pt_index] & ~0xFFF) | offset);
  ;
}

uint64_t *phys_to_virt(uint64_t phys_addr)
{
  return (uint64_t *)((phys_addr) + 0xFFFFFFFF80000000);
}

void clean_new_table(uint64_t *table_to_clean)
{
  for (int i = 0; i < 512; i++)
  {
    table_to_clean[i] = 0;
  }
}

int is_virtual_memory_mapped(uint64_t virtual_address)
{

  virtual_address = align_to_page(virtual_address);

  uint16_t pml4_e = PML4_ENTRY((uint64_t)virtual_address);
  uint16_t pdpr_e = PDPR_ENTRY((uint64_t)virtual_address);
  uint16_t pd_e = PD_ENTRY((uint64_t)virtual_address);

  uint64_t *pml4_table = (uint64_t *)(SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, 510l, 510l));
  uint64_t *pdpr_table = (uint64_t *)(SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, 510l, (uint64_t)pml4_e));
  uint64_t *pd_table = (uint64_t *)(SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, (uint64_t)pml4_e, (uint64_t)pdpr_e));

  uint64_t *pt_table = (uint64_t *)(SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, (uint64_t)pml4_e, (uint64_t)pdpr_e, (uint64_t)pd_e));
  uint16_t pt_e = PT_ENTRY((uint64_t)virtual_address);

  if (!(pml4_table[pml4_e] & PT_PRESENT_BIT))
  {
    return 0;
  }

  if (!(pdpr_table[pdpr_e] & PT_PRESENT_BIT))
  {
    return 0;
  }

  if (!(pd_table[pd_e] & PT_PRESENT_BIT))
  {
    return 0;
  }
  if (!(pt_table[pt_e] & PT_PRESENT_BIT))
  {
    return 0;
  }

  return 1;
}

void map_if_not_mapped(uint64_t address)
{
  address = align_to_nearest_page(address);
  if (is_virtual_memory_mapped(address) == 0)
  {
    printf("HALP!!!\n");
    uint64_t paddr = allocate_physical_page();
    map_page(address, paddr, p4_table, PT_PRESENT_BIT | PT_RW_BIT);
  }
}

void map_page(uint64_t virtual_address, uint64_t physical_address, uint64_t *pml4_tassssble, uint8_t flags)
{
  virtual_address = align_to_page(virtual_address);
  physical_address = align_to_page(physical_address);

  uint16_t pml4_e = PML4_ENTRY((uint64_t)virtual_address);
  uint16_t pdpr_e = PDPR_ENTRY((uint64_t)virtual_address);
  uint16_t pd_e = PD_ENTRY((uint64_t)virtual_address);

  uint64_t *pml4_table = (uint64_t *)(SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, 510l, 510l));
  uint64_t *pdpr_table = (uint64_t *)(SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, 510l, (uint64_t)pml4_e));
  uint64_t *pd_table = (uint64_t *)(SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, (uint64_t)pml4_e, (uint64_t)pdpr_e));

  printfs("Map phys to virtual:  Pml4: %u - pdpr: %u - pd: %u - flags: 0x%x to address: 0x%x\n", pml4_e, pdpr_e, pd_e, flags, virtual_address);
  uint64_t *pt_table = (uint64_t *)(SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, (uint64_t)pml4_e, (uint64_t)pdpr_e, (uint64_t)pd_e));
  uint16_t pt_e = PT_ENTRY((uint64_t)virtual_address);

  // If the pml4_e item in the pml4 table is not present, we need to create a new one.
  // Every entry in pml4 table points to a pdpr table
  if (!(pml4_table[pml4_e] & PT_PRESENT_BIT))
  {
    uint64_t *new_table = allocate_physical_page();

    pml4_table[pml4_e] = (uint64_t)new_table | PT_RW_BIT | PT_PRESENT_BIT;
    printfs("Need to allocate pml4 for address: 0x%x - Entry value: 0x%x - phys_address: 0x%x", (uint64_t)virtual_address, pml4_table[pml4_e], new_table);
    clean_new_table(pdpr_table);
  }

  if (!(pdpr_table[pdpr_e] & PT_PRESENT_BIT))
  {
    uint64_t *new_table = allocate_physical_page();
    pdpr_table[pdpr_e] = (uint64_t)new_table | PT_RW_BIT | PT_PRESENT_BIT;
    printfs("PDPR entry value: 0x%x", pdpr_table[pdpr_e]);
    clean_new_table(pd_table);
  }

  // If the pd_e item in the pd table is not present, we need to create a new one.
  // Every entry in pdpr table points to a page table if using 4k pages, or to a 2mb memory area if using 2mb pages
  if (!(pd_table[pd_e] & PT_PRESENT_BIT))
  {

    uint64_t *new_table = allocate_physical_page();
    pd_table[pd_e] = (uint64_t)new_table | PT_RW_BIT | PT_PRESENT_BIT;
    clean_new_table(pt_table);
  }

  // This case apply only for 4kb pages, if the pt_e entry is not present in the page table we need to allocate a new 4k page
  // Every entry in the page table is a 4kb page of physical memory
  if (!(pt_table[pt_e] & PT_PRESENT_BIT))
  {
    pt_table[pt_e] = (uint64_t)physical_address | flags;
  }

  return;
}
