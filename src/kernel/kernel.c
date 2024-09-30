#include "multiboot.h"
#include <types.h>
#include <drivers/serial.h>
#include <drivers/vga.h>
#include <stdio.h>
#include <string.h>
#include <arch/x86_64/idt/idt.h>

int kernel_main(uint32_t addr, uint32_t magic)
{
	vga_clear_screen();
	serial_init();

	idt_init();

	puts("Ohai, world!");

	while (1)
		__asm__("hlt");
}