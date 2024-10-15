#ifndef DRIVERS_VGA_H_
#define DRIVERS_VGA_H_

#include <types.h>

// #ifdef USE_FRAMEBUFFER
#define VGA_VIDEO_MEM_START (0xffffffffbd1D8000)
// #else
// #define VGA_VIDEO_MEM_START 0xFFFFFFFF800b8000
// #endif

enum vga_color
{
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

extern void vga_clear_vga_screen();
extern void vga_write_char(int ch, int x, int y);

extern void vga_set_cursor_pos(int x, int y);
extern void vga_get_cursor_pos(int *x, int *y);

extern uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
extern void vga_scroll_screen();
extern void vga_next_line();

extern void enable_cursor(uint8_t start, uint8_t end);

extern void vga_resize(int new_width, int new_height);
extern uint16_t *vga_get_buffer();

#endif // DRIVERS_VGA_H_