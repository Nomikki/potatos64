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


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

extern int test_pmode();


uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}


uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}


int kernel_main() {
	uint16_t * terminal_buffer = (uint16_t*) 0xB8000;	

	uint8_t terminal_color =  vga_entry_color(VGA_COLOR_RED, VGA_COLOR_WHITE);

	if (test_pmode() == 1) {
		terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_WHITE);
	} else 
	{
		terminal_color = vga_entry_color(VGA_COLOR_RED, VGA_COLOR_WHITE);
	}

	terminal_buffer[0] =  vga_entry('H', terminal_color);
	terminal_buffer[1] =  vga_entry('e', terminal_color);
	terminal_buffer[2] =  vga_entry('i', terminal_color);
	terminal_buffer[3] =  vga_entry('!', terminal_color);

	return 0;
}