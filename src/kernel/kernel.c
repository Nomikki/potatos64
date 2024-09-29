#include "multiboot.h"
#include <types.h>
#include <drivers/serial.h>

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};


uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}


uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

struct Char
{
    uint8_t ch;
    uint8_t col;
};

int kernel_main(uint32_t addr, uint32_t magic) {
	//uint16_t * terminal_buffer = (uint16_t*) 0xFFFFFFFF800b8000;	
	
	#define _VIDEO_MEM_START 0xFFFFFFFF800b8000
	struct Char *buffer = (struct Char *)_VIDEO_MEM_START;

	uint8_t terminal_color =  vga_entry_color(VGA_COLOR_RED, VGA_COLOR_WHITE);

	buffer[0] =  (struct Char){(uint8_t)'H', terminal_color};
	buffer[1] =  (struct Char){(uint8_t)'e', terminal_color};
	buffer[2] =  (struct Char){(uint8_t)'i', terminal_color};
	buffer[3] =  (struct Char){(uint8_t)'!', terminal_color};

	serial_init();
	serial_writeline("Ruaah!");
	serial_writeline("Kokeillaan uudestaan!");

	while(1) {};

	return 0;
	
}