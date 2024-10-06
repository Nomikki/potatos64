#include <mem/mmap.h>
#include <stdio.h>

uint32_t mmap_number_of_entries;
multiboot_memory_map_t *mmap_entries;

const char *mmap_types[] = {
    "Invalid",
    "Available",
    "Reserved",
    "Reclaimable",
    "NVS",
    "Defective"};

void mmap_parse(struct multiboot_tag_mmap *mmap_root)
{
  int total_entries = 0;
  mmap_number_of_entries = (mmap_root->size - sizeof(*mmap_root)) / mmap_root->entry_size;
  printf("mmap: num of entries: %u\n\n", mmap_number_of_entries);

  mmap_entries = (struct multiboot_memory_map_t *)mmap_root->entries;
  for (int i = 0; i < mmap_number_of_entries; i++)
  {
    multiboot_memory_map_t *current_entry = &mmap_entries[i];
    printf("%i: %p:%p len: %x - type: %i [%s]\n", i, current_entry->addr, current_entry->addr + current_entry->len, current_entry->len, current_entry->type, mmap_types[current_entry->type]);
    total_entries++;

    if (current_entry->type == MULTIBOOT_MEMORY_AVAILABLE)
    {
    }
  }

  printf("Total entries: %i\n", total_entries);
}
