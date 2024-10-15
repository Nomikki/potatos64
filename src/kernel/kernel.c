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

#define _HIGHER_HALF_KERNEL_MEM_START 0xffffffff80000000

extern uint64_t multiboot_framebuffer_data;
extern uint64_t multiboot_basic_meminfo;
extern uint64_t multiboot_mmap_data;

struct multiboot_tag_basic_meminfo *tagmem = NULL;
struct multiboot_tag_framebuffer *tagfb = NULL;
struct multiboot_tag_mmap *tagmmap = NULL;

uint64_t *bitmap_address = NULL;
extern uint64_t _kernel_end;
extern uint64_t _kernel_physical_end;
int64_t all_memory = 0;

extern uint64_t p4_table[];

void init_memory()
{
	tagmem = (struct multiboot_tag_basic_meminfo *)(multiboot_basic_meminfo + _HIGHER_HALF_KERNEL_MEM_START);
	all_memory = tagmem->mem_lower + tagmem->mem_upper;

	tagmmap = (struct multiboot_tat_mmap *)(multiboot_mmap_data + _HIGHER_HALF_KERNEL_MEM_START);

	printf("Memory available: : (%u MB), (%u KB)\n", all_memory / 1024, all_memory);

	parse_mmap(tagmmap);

	// find area for bitmap and return its address. If not enough memory/error, return NULL.
	bitmap_address = (void *)(init_bitmap(all_memory / (4 * 16))) - _HIGHER_HALF_KERNEL_MEM_START;

	// then find all reserved areas and mark them to bitmap

	printf("Kernel physical end: %p\n", &_kernel_physical_end);
	printf("Bitmap address: %p\n", bitmap_address);
	setup_paging();
}

// just placeholder
void draw_bitmap()
{

	uint32_t bitmap_size = all_memory / (4 * 16);
	uint8_t c = 64;
	uint64_t test_page = 0;

	if (bitmap_size > 512)
		bitmap_size = 512;

	for (int i = 0; i < bitmap_size; i++)
	{
		for (int x = 0; x < 16; x++)
		{

			if (is_page_free(test_page) == 0)
				c = 255;
			else
				c = 0;

			int y = (i + 16) % 600;

			plot_pixel(800 - 64 + x, y, c, c, c);
			test_page++;
		}
	}
}

void init_video()
{
	tagfb = (struct multiboot_tag_framebuffer *)(multiboot_framebuffer_data + _HIGHER_HALF_KERNEL_MEM_START);
	vga_resize(100, 37);
	/*
	printf("framebuffer addr: %p\n", (tagfb->common.framebuffer_addr + _HIGHER_HALF_KERNEL_MEM_START));
	printf("framebuffer width: %i\n", tagfb->common.framebuffer_width);
	printf("framebuffer height: %i\n", tagfb->common.framebuffer_height);
	printf("framebuffer bpp: %i\n", tagfb->common.framebuffer_bpp);
	printf("framebuffer pitch: %i\n", tagfb->common.framebuffer_pitch);
	printf("framebuffer type: %i\n", tagfb->common.framebuffer_type);
	*/
	printf("\n");
	init_framebuffer();
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
	init_serial();
	init_idt();
	init_video();
	init_memory();

	// printf("location of main: %p\n", (void *)kernel_main - _HIGHER_HALF_KERNEL_MEM_START);

	// hexdump(vga_get_buffer(), (vga_get_buffer() + 200));

	while (1)
	{
		clear_framebuffer(26, 27, 38);
		draw_text(0, 0, "PotatOS", 255, 255, 255);

		draw_vga_buffer(vga_get_buffer(), 100, 37);
		draw_bitmap();

		flip_framebuffer();
	}

	while (1)
		__asm__("hlt");
}
