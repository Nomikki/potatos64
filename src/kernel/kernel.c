#include "multiboot.h"
#include <types.h>
#include <drivers/serial.h>
#include <drivers/vga.h>
#include <stdio.h>
#include <string.h>
#include <arch/x86_64/idt/idt.h>

void test()
{
}

int kernel_main(uint32_t addr, uint32_t magic)
{
	vga_clear_screen();
	serial_init();

	idt_init();

	puts("Ohai, world!");
	printf("Moi! %p\n", &kernel_main);
	printf("Moi! %p\n", addr);
	for (int i = 0; i < 1024; i++)
	{
		printf("i: %x \n", i);
	}

	while (1)
		__asm__("hlt");
}