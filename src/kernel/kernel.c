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
	enable_cursor(14, 15);
	serial_init();

	idt_init();

	printf("%x\n", addr);
	printf("%x\n", addr);
	printf("%b\n", addr);

	while (1)
		__asm__("hlt");
}