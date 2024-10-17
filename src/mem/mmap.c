#include <mem/mmap.h>
#include <mem/bitmap.h>
#include <drivers/serial.h>

#include <stdio.h>

uint32_t mmap_number_of_entries;
multiboot_memory_map_t *mmap_entries;

const char *mmap_types[] = {
    "Invalid          ",
    "Available        ",
    "Reserved         ",
    "ACPI Reclaimable ",
    "NVS              ",
    "Defective        "};

void parse_mmap(struct multiboot_tag_mmap *mmap_root, uint64_t size_of_memory)
{
  int total_entries = 0;
  uint64_t free_memory = 0;
  mmap_number_of_entries = (mmap_root->size - sizeof(*mmap_root)) / mmap_root->entry_size;
  printfs("mmap: num of entries: %u\n\n", mmap_number_of_entries);

  mmap_entries = (struct multiboot_memory_map_t *)mmap_root->entries;

  for (int i = 0; i < mmap_number_of_entries; i++)
  {
    multiboot_memory_map_t *current_entry = &mmap_entries[i];
    printfs("%i: %p:%p - type: %i [%s]: len: %u KB\n", i, current_entry->addr, current_entry->addr + current_entry->len, current_entry->type, mmap_types[current_entry->type], current_entry->len / 1024);
    total_entries++;

    if (current_entry->type == MULTIBOOT_MEMORY_AVAILABLE)
    {
      free_memory += current_entry->len;
    }
    if (current_entry->type == MULTIBOOT_MEMORY_RESERVED)
    {
      if (current_entry->addr + current_entry->len > size_of_memory)
        break;
      set_page_range_used(current_entry->addr, current_entry->addr + current_entry->len);
    }
  }

  printfs("Total entries: %i\n", total_entries);
  printfs("Free memory: %u KB\n", free_memory / 1024);
}
