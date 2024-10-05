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

	printf("Memory available: : (%i MB), (%i KB)\n", all_memory / 1024, all_memory);
}

void init_video()
{
	tagfb = (struct multiboot_tag_framebuffer *)(multiboot_framebuffer_data + _HIGHER_HALF_KERNEL_MEM_START);
	vga_resize(100, 37);
	printf("framebuffer addr: %p\n", (tagfb->common.framebuffer_addr + _HIGHER_HALF_KERNEL_MEM_START));
	printf("framebuffer width: %i\n", tagfb->common.framebuffer_width);
	printf("framebuffer height: %i\n", tagfb->common.framebuffer_height);
	printf("framebuffer bpp: %i\n", tagfb->common.framebuffer_bpp);
	printf("framebuffer pitch: %i\n", tagfb->common.framebuffer_pitch);
	printf("framebuffer type: %i\n", tagfb->common.framebuffer_type);
	printf("\n");
	framebuffer_init();
}

void hexdump(const void *start, const void *end)
{
	const uint8_t *ptr = (const uint8_t *)start;
	const uint8_t *end_ptr = (const uint8_t *)end;

	while (ptr < end_ptr)
	{
		printf("%p: ", ptr);

		for (int i = 0; i < 16; i++)
		{
			if (ptr + i < end_ptr)
			{
				if (ptr[i] >= 10)
					printf("%x ", ptr[i]);
				else
					printf("0%x ", ptr[i]);
			}
			else
			{
				printf("   ");
			}
		}

		printf(" | ");
		for (int i = 0; i < 16; i++)
		{
			if (ptr + i < end_ptr)
			{
				char ch = ptr[i];
				printf("%c", (ch >= 32 && ch < 127) ? ch : '.');
			}
			else
			{
				printf(" ");
			}
		}

		printf("\n");
		ptr += 16;
	}
}

int kernel_main(uint32_t addr, uint32_t magic)
{
	vga_clear_screen();
	enable_cursor(14, 15);
	serial_init();
	idt_init();
	init_memory();
	init_video();

	hexdump(vga_get_buffer(), (vga_get_buffer() + 200));

	while (1)
	{
		framebuffer_clear(26, 27, 38);
		draw_text(0, 0, "PotatOS", 255, 255, 255);

		videobuffer_draw_vga_buffer(vga_get_buffer(), 100, 37);
		framebuffer_flip();
	}

	while (1)
		__asm__("hlt");
}