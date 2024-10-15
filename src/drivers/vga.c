#include <drivers/vga.h>

uint16_t *vga_buffer = (uint16_t *)VGA_VIDEO_MEM_START;
int16_t cursorX = 0;
int16_t cursorY = 0;
static uint8_t vga_color = 0;

int VGA_WIDTH = 80;
int VGA_HEIGHT = 25;

bool redirect_to_videomemory = false;

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
  return fg | bg << 4;
}

uint16_t vga_entry(uint8_t ch, uint8_t color)
{
  return (uint16_t)ch | (uint16_t)color << 8;
}

void vga_set_color(uint8_t c)
{
  vga_color = c;
}

void vga_clear_screen()
{
  uint32_t max_rows = VGA_HEIGHT * VGA_WIDTH;
  vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

  for (int i = 0; i < max_rows; i++)
  {
    vga_buffer[i] = vga_entry(0, vga_color);
  }

  vga_set_cursor_pos(0, 0);
}

void vga_check_if_scroll_needed()
{
  while (cursorY >= VGA_HEIGHT - 1)
  {
    vga_scroll_screen();
    cursorY--;
  }
}

void vga_next_line()
{
  cursorX = 0;
  cursorY++;
  vga_check_if_scroll_needed();
}

void vga_write_char(int ch, int x, int y)
{
  if (ch == '\n')
  {
    vga_next_line();
    vga_update_cursor();
    return;
  }

  if (ch == 14) // go one step to left / erase
  {
    if (cursorX > 0)
      cursorX--;
    int i = y * VGA_WIDTH + cursorX;

    vga_buffer[i] = vga_entry(' ', vga_color);
    vga_update_cursor();
    return;
  }

  int i = y * VGA_WIDTH + x;

  if (i < VGA_WIDTH * VGA_HEIGHT)
  {
    vga_buffer[i] = vga_entry(ch, vga_color);
  }
  cursorX++;

  if (cursorX >= VGA_WIDTH)
  {
    vga_next_line();
  }
  vga_update_cursor();
}

void set_cursor_position(uint16_t position)
{
  outportb(0x3D4, 0x0F);                       // Lähetetään kursoriin liittyvä alhaisempi tavu
  outportb(0x3D5, (uint8_t)(position & 0xFF)); // Alempi tavu sijainnista

  outportb(0x3D4, 0x0E);                              // Lähetetään kursoriin liittyvä korkeampi tavu
  outportb(0x3D5, (uint8_t)((position >> 8) & 0xFF)); // Korkeampi tavu sijainnista
}

void enable_cursor(uint8_t start, uint8_t end)
{
  outportb(0x3D4, 0x0A);
  outportb(0x3D5, (inportb(0x3D5) & 0xC0) | start);

  outportb(0x3D4, 0x0B);
  outportb(0x3D5, (inportb(0x3D5) & 0xE0) | end);

  vga_update_cursor();
}

void vga_update_cursor()
{
  uint16_t i = cursorY * VGA_WIDTH + cursorX;
  set_cursor_position(i);
}

void vga_set_cursor_pos(int x, int y)
{
  cursorX = x;
  cursorY = y;
  vga_update_cursor();
}

void vga_get_cursor_pos(int *x, int *y)
{
  *x = cursorX;
  *y = cursorY;
}

void vga_scroll_screen()
{
  int max_row_minus_one = VGA_WIDTH * VGA_HEIGHT - VGA_WIDTH;
  for (int i = 0; i < max_row_minus_one; i++)
  {
    vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
  }

  for (int i = 0; i < VGA_WIDTH; i++)
  {
    vga_buffer[max_row_minus_one + i] = 0;
  }
}

void vga_resize(int new_width, int new_height)
{
  VGA_WIDTH = new_width;
  VGA_HEIGHT = new_height;
}

uint16_t *vga_get_buffer()
{
  return vga_buffer;
}