#ifndef __mem__bitmap__
#define __mem__bitmap__

#include <types.h>

extern uint64_t *bitmap_init(uint32_t size);
extern void bitmap_set_page(uint64_t page_address, bool present);
extern int bitmap_test(uint64_t address);
#endif