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
#include <scheluding/scheluder.h>

#define _HIGHER_HALF_KERNEL_MEM_START 0xffffffff80000000

extern uint64_t multiboot_framebuffer_data;
struct multiboot_tag_framebuffer *tagfb = NULL;

extern process_t processes_list[32];

void init_memory()
{
	// Initialize physical memory manager
	init_pmm();

	// Initialize virtual memory manager
	init_vmm();
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
	init_vga();
	init_video();
	init_idt();
	init_memory();

	init_scheluder();

	idt_activate();

	while (1)
	{
		draw_background_image();
		draw_text(0, 0, "PotatOS", 255, 255, 255);
		draw_vga_buffer(vga_get_buffer(), 100, 37);
		flip_framebuffer();

		__asm__("hlt");
	}

	while (1)
		__asm__("hlt");
}
