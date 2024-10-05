#include "multiboot.h"
#include <types.h>
#include <drivers/serial.h>
#include <drivers/vga.h>
#include <stdio.h>
#include <string.h>
#include <arch/x86_64/idt/idt.h>
#include <drivers/framebuffer/framebuffer.h>

#define _HIGHER_HALF_KERNEL_MEM_START 0xffffffff80000000

extern uint64_t multiboot_framebuffer_data;
extern uint64_t multiboot_basic_meminfo;

struct multiboot_tag_basic_meminfo *tagmem = NULL;
struct multiboot_tag_framebuffer *tagfb = NULL;

void init_memory()
{
	tagmem = (struct multiboot_tag_basic_meminfo *)(multiboot_basic_meminfo + _HIGHER_HALF_KERNEL_MEM_START);
	uint64_t all_memory = tagmem->mem_lower + tagmem->mem_upper;

	printfs("Memory available: : (%i MB), (%i KB)\n", all_memory / 1024, all_memory);
}

void init_video()
{
	tagfb = (struct multiboot_tag_framebuffer *)(multiboot_framebuffer_data + _HIGHER_HALF_KERNEL_MEM_START);

	printfs("framebuffer addr: %p\n", (tagfb->common.framebuffer_addr + _HIGHER_HALF_KERNEL_MEM_START));
	printfs("framebuffer width: %i\n", tagfb->common.framebuffer_width);
	printfs("framebuffer height: %i\n", tagfb->common.framebuffer_height);
	printfs("framebuffer bpp: %i\n", tagfb->common.framebuffer_bpp);
	printfs("framebuffer pitch: %i\n", tagfb->common.framebuffer_pitch);
	printfs("framebuffer type: %i\n", tagfb->common.framebuffer_type);
}

int kernel_main(uint32_t addr, uint32_t magic)
{
	vga_clear_screen();
	enable_cursor(14, 15);
	serial_init();
	idt_init();
	init_memory();
	init_video();

	int k = 0;
	while (1)
	{
		framebuffer_clear(26, 27, 38);
		k++;
		k = k % 255;

		for (int k = 0; k < 32; k++)
			plot_pixel(32 + k, 32, 255, 255, 255);

		framebuffer_flip();
	}

	while (1)
		__asm__("hlt");
}