#include <drivers/vga.h>

static uint16_t *vga_buffer = (uint16_t *)VGA_VIDEO_MEM_START;
static uint8_t cursorX = 0;
static uint8_t cursorY = 0;
static uint8_t vga_color = 0;

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
    vga_buffer[i] = vga_entry(' ', vga_color);
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
}

void vga_set_cursor_pos(int x, int y)
{
  cursorX = x;
  cursorY = y;
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