#include "multiboot.h"
#include <types.h>
#include <drivers/serial.h>
#include <drivers/vga.h>
#include <stdio.h>
#include <string.h>
#include <arch/x86_64/idt/idt.h>
#include <drivers/framebuffer/framebuffer.h>
#include <mem/pmm.h>
#include <mem/mmap.h>
#include <mem/paging.h>
#include <mem/vmm.h>
#include <mem/kheap.h>

#define _HIGHER_HALF_KERNEL_MEM_START 0xffffffff80000000

extern uint64_t multiboot_framebuffer_data;
extern uint64_t multiboot_basic_meminfo;
extern uint64_t multiboot_mmap_data;
extern uint64_t multiboot_acpi_info;
extern uint64_t stack;

struct multiboot_tag_framebuffer *tagfb = NULL;
struct multiboot_tag_basic_meminfo *tagmem = NULL;
struct multiboot_tag_mmap *tagmmap = NULL; // This tag provides memory map.
struct multiboot_tag_old_acpi *tagacpiold = NULL;
struct multiboot_tag_new_acpi *tagacpinew = NULL;

uint64_t *bitmap_address = NULL;
extern uint64_t _kernel_start;
extern uint64_t _kern_virtual_offset;
extern uint64_t end_of_mapped_memory;
extern uint64_t _kernel_end;
extern uint64_t _kernel_physical_end;
int64_t all_memory = 0;

extern uint64_t p4_table[];

void init_memory()
{
	/* From multiboot2 doc:
		‘mem_lower’ and ‘mem_upper’ indicate the amount of lower and upper memory, respectively, in kilobytes.
		Lower memory starts at address 0, and upper memory starts at address 1 megabyte.
		The maximum possible value for lower memory is 640 kilobytes.
		The value returned for upper memory is maximally the address of the first upper memory hole minus 1 megabyte.
		It is not guaranteed to be this value.
	*/
	tagmem = (struct multiboot_tag_basic_meminfo *)(multiboot_basic_meminfo + _HIGHER_HALF_KERNEL_MEM_START);
	all_memory = tagmem->mem_lower + tagmem->mem_upper;

	// find area for bitmap and return its address. If not enough memory/error, return NULL.
	bitmap_address = (void *)(init_bitmap(all_memory * 1024)) - _HIGHER_HALF_KERNEL_MEM_START;

	/* From multiboot2 doc:
		The map provided is guaranteed to list all standard RAM that should be available for normal use.
		This type however includes the regions occupied by kernel, mbi, segments and modules.
		Kernel must take care not to overwrite these regions.
	*/
	tagmmap = (struct multiboot_tat_mmap *)(multiboot_mmap_data + _HIGHER_HALF_KERNEL_MEM_START);
	parse_mmap(tagmmap, all_memory * 1024);

	printf("Memory available: : (%u MB), (%u KB)\n", all_memory / 1024, all_memory);
	printf("Kernel physical end: %p\n", &_kernel_physical_end);
	printf("Bitmap address: %p\n", bitmap_address);
	printf("end_of_mapped_memory: %p\n", &end_of_mapped_memory);

	// Initialize virtual memory manager
	init_vmm();

	// and finally heap for the kernel <3
	init_kheap();
}

void init_video()
{
	tagfb = (struct multiboot_tag_framebuffer *)(multiboot_framebuffer_data + _HIGHER_HALF_KERNEL_MEM_START);
	vga_resize(100, 37);
	init_framebuffer();
}

int kernel_main(uint32_t addr, uint32_t magic)
{
	init_serial();
	init_idt();
	init_vga();
	init_video();
	init_memory();

	// printf("location of main: %p\n", (void *)kernel_main - _HIGHER_HALF_KERNEL_MEM_START);
	// printf("Kernel start: %p\n", &_kernel_start);
	// printf("_kern_virtual_offset: %p\n", &_kern_virtual_offset);

	while (1)
	{
		// clear_framebuffer(26, 27, 38);
		draw_background_image();
		draw_text(0, 0, "PotatOS", 255, 255, 255);
		draw_vga_buffer(vga_get_buffer(), 100, 37);
		flip_framebuffer();
	}

	while (1)
		__asm__("hlt");
}
