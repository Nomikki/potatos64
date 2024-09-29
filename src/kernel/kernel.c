#include "multiboot.h"
#include <types.h>
#include <drivers/serial.h>
#include <drivers/vga.h>
#include <stdio.h>
#include <string.h>

int kernel_main(uint32_t addr, uint32_t magic)
{
	vga_clear_screen();
	serial_init();

	puts("Ohai, world!");
	puts("No, morroo!");

	return 0;
}