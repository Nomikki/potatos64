#ifndef __drivers__vga__
#define __drivers__vga__

extern void vga_clear_screen();
extern void vga_write_char(int ch, int x, int y);
extern void vga_write_string(const char *str, int x, int y);
extern void vga_set_cursor(int x, int y);

#endif