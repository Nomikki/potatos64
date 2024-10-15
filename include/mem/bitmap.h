#ifndef __mem__bitmap__
#define __mem__bitmap__

#include <types.h>

extern uint64_t *bitmap_init(uint32_t size);
// extern void bitmap_set_page(uint64_t page_address, bool present);
// extern int bitmap_test(uint64_t address);
extern int is_page_free(uint64_t page_index);
extern void set_page_used(uint64_t page_index);
extern uint64_t allocate_physical_page();
extern void set_page_range_used(uint64_t memory_range_start, uint64_t memory_range_end);

#endif