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

#define ONE_GIGABYTE 0x40000000

vmm_region *vm_root = NULL;

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

uint64_t align_to_nearest_page(uint64_t value)
{
  return ((value + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
}

void vmm_allocate_area(uint64_t new_virtual_address, uint64_t size, vmm_region *current, vmm_region *prev)
{
  for (uint64_t i = 0; i < size; i += PAGE_SIZE)
  {
    uint64_t physical_address = allocate_physical_page();
    map_page(new_virtual_address + i, physical_address, p4_table, PT_PRESENT_BIT | PT_RW_BIT);
  }
}

uint64_t vmm_allocate(uint64_t size, uint8_t flags)
{
  flags = convert_x86_64_vm_flags(flags);

  // align to nearest page
  size = align_to_nearest_page(size + (sizeof(vmm_region)));

  // printf("vmm_allocate: aligned size: %u bytes [%u KB, %u MB]: pages needed: %u\n", size, size / 1024, size / 1024 / 1024, size / PAGE_SIZE);
  vmm_region *current = vm_root;
  vmm_region *prev = NULL;

  if (current->size == 0)
    current->size = PAGE_SIZE;

  while (current != NULL)
  {
    if (current == NULL)
      break;

    if (prev != NULL)
    {
      uint64_t gap_area = (uint64_t)(&current->next) - ((uint64_t)(prev) + prev->size);
      if (gap_area >= size)
      {
        current = (uint64_t)(prev) + prev->size;
        current->size = size;
        current->next = prev->next;
        prev->next = current;

        vmm_allocate_area(current, size, current, prev);
        current->size = size;
        prev->next = current;

        // ensure that the area is mapped
        return current + (sizeof(vmm_region));
      }
    }

    prev = current;
    current = current->next;
  }

  uint64_t new_virtual_address = (uint64_t)(prev) + prev->size;

  vmm_allocate_area(new_virtual_address, size, current, prev);
  current = new_virtual_address;
  current->next = 0;
  current->size = size;
  prev->next = current;

  return new_virtual_address + (sizeof(vmm_region));
}

void vmm_free(uint64_t *address)
{
  // printf("Free allocation from: %p\n", (uint64_t)address - (sizeof(vmm_region)));

  vmm_region *current = vm_root;
  vmm_region *prev = NULL;

  uint64_t target_address = (uint64_t)address - sizeof(vmm_region);

  while (current != NULL)
  {
    // printf("current: %p\n", current);
    if (current == NULL)
      break;

    if (current == target_address)
    {
      // printf("Memory area found!\n");
      //  unmap it!
      prev->next = current->next;
      uint64_t physical_address = (uint64_t *)get_physical_address(target_address);
      // printf("Physical address: %p\n", (uint64_t)physical_address);
      set_page_free(physical_address / PAGE_SIZE);
      map_page(target_address, 0, p4_table, 0);
      return;
    }

    prev = current;
    current = current->next;
  }

  // printf("JAIKS!\n");
}

void check_allocations()
{
  vmm_region *current = vm_root;
  // printf("walk:\n");
  uint64_t steps = 0;
  while (current != NULL)
  {
    if (current == NULL)
      break;
    printf("addr: %p, size: %u\n", current, current->size);
    printf("   -> %p\n", current->next);
    current = current->next;
    steps++;

    if (steps > 1024) // just give up. We failed. :F
      break;
  }
  printf("Active allocations: %i\n", steps);
}

void allocation_test()
{

  printf("Allocation test: \n");
  uint64_t ptr_array[100];
  ptr_array[0] = vmm_allocate((1024 * 1), VM_FLAG_EXEC | VM_FLAG_WRITE);
  ptr_array[1] = vmm_allocate((1024 * 8), VM_FLAG_EXEC | VM_FLAG_WRITE);

  printf("original:\n");
  check_allocations();

  vmm_free(ptr_array[1]);

  for (int i = 0; i < 10; i++)
    ptr_array[i] = vmm_allocate((1024 * 1), VM_FLAG_EXEC | VM_FLAG_WRITE);

  for (int i = 0; i < 10; i++)
    vmm_free(ptr_array[i]);

  printf("after alloc/free combo:\n");
  check_allocations();

  printf("last one:\n");
  ptr_array[0] = vmm_allocate((1024 * 8), VM_FLAG_EXEC | VM_FLAG_WRITE);

  check_allocations();
}

void init_vmm()
{
  printf("Init VMM\n");

  uint64_t vmm_address = align_to_nearest_page(&vmm_area);
  // make some room for linked list
  uint64_t vmm_start_physical_address = allocate_physical_page();
  printf("VMM area: %p: %p\n", vmm_address, vmm_start_physical_address);

  set_page_used(vmm_start_physical_address / 0x1000);
  map_page(vmm_address, vmm_start_physical_address, p4_table, PT_PRESENT_BIT | PT_RW_BIT);
  vm_root = (vmm_region *)vmm_address;
  vm_root->size = PAGE_SIZE;

  // printf("vmm root: %p\n", (void *)vm_root);
  //  printf("size of node: %u\n", sizeof(vmm_region));
  // allocation_test();

  // and finally heap for the kernel <3
  init_kheap();
  printf("end_of_mapped_memory: %p\n", &end_of_mapped_memory);

  printf("VMM init OK\n\n");
}
