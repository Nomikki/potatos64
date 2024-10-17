#ifndef MEM_BITMAP_H_
#define MEM_BITMAP_H_
#include <types.h>

#define BITS_PER_ROW 8
#define PAGE_SIZE 4096

extern uint64_t *init_bitmap(uint64_t size);
extern int is_page_free(uint64_t page_index);
extern void set_page_used(uint64_t page_index);
extern uint64_t allocate_physical_page();
extern void set_page_range_used(uint64_t memory_range_start, uint64_t memory_range_end);

#endif // MEM_BITMAP_H_