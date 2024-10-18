#include <drivers/framebuffer/framebuffer.h>
#include <multiboot.h>
#include <drivers/serial.h>
#include <mem/paging.h>
#include <utils/background.h>

PSF_font *font;

extern uint64_t fbb_pt_tables;
uint32_t *framebuffer = (uint32_t *)0xffffffffbd000000;

uint32_t framebuffer_buffer[800 * 600];
// uint32_t *framebuffer_buffer = (uint32_t *)(0xffffffffbd000000 + (800 * 600 * 4));

extern struct multiboot_tag_framebuffer *tagfb;
extern char _binary_font_psf_start;
int font_bytesPerLine;

void init_framebuffer()
{
  font = (PSF_font *)&_binary_font_psf_start;
  font_bytesPerLine = (font->width + 7) / 8;
  printfs("Framebuffer:\n");
  printfs("framebuffer width: %i\n", tagfb->common.framebuffer_width);
  printfs("framebuffer height: %i\n", tagfb->common.framebuffer_height);
  printfs("framebuffer bpp: %i\n", tagfb->common.framebuffer_bpp);
  printfs("framebuffer pitch: %i\n", tagfb->common.framebuffer_pitch);
  printfs("framebuffer type: %i\n", tagfb->common.framebuffer_type);

  printfs("\n");

  printfs("framebuffer: %p -> %p\n", (void *)framebuffer, (void *)get_physical_address((void *)framebuffer));
  printfs("framebuffer_buffer: %p -> %p\n", (void *)framebuffer_buffer, get_physical_address((void *)framebuffer_buffer));
  printfs("framebuffer: %p -> %p\n", (void *)&fbb_pt_tables, get_physical_address((void *)&fbb_pt_tables));
  printfs("\n");
}

void clear_framebuffer(uint8_t r, uint8_t g, uint8_t b)
{
  uint64_t size = tagfb->common.framebuffer_width * tagfb->common.framebuffer_height;
  uint32_t color = (r << 16) | (g << 8) | b;

  for (uint32_t i = 0; i < size; i += 4)
  {
    framebuffer_buffer[i] = color;
    framebuffer_buffer[i + 1] = color;
    framebuffer_buffer[i + 2] = color;
    framebuffer_buffer[i + 3] = color;
  }
}

void draw_character(uint8_t ch, int cx, int cy, uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t *glyph = (uint8_t *)&_binary_font_psf_start + font->headersize + (ch > 0 && ch < font->numglyph ? ch : 0) * font->bytesperglyph;

  int mask;
  for (uint32_t y = 0; y < font->height; y++)
  {
    mask = 1 << (font->width - 1);
    for (uint32_t x = 0; x < font->width; x++)
    {
      if (*((unsigned int *)glyph) & mask)
      {
        plot_pixel(cx + x, cy + y, r, g, b);
      }

      mask >>= 1;
    }

    glyph += font_bytesPerLine;
  }
}

void draw_text(int cx, int cy, const char *text, uint8_t r, uint8_t g, uint8_t b)
{
  int len = strlen(text);

  for (int i = 0; i < len; i++)
  {
    uint8_t ch = text[i];
    draw_character(ch, cx + (i * font->width), cy, r, g, b);
  }
}

void draw_vga_buffer(uint16_t *vga_buf, int width, int height)
{
  uint8_t r = 160;
  uint8_t g = 160;
  uint8_t b = 160;

  for (int py = 0; py < height; py++)
  {

    char tempText[width + 1];
    for (int px = 0; px < width + 1; px++)
      tempText[px] = 0;

    for (int px = 0; px < width; px++)
    {
      tempText[px] = vga_buf[px + (py * width)];
    }
    draw_text(0, py * 16 + 16, tempText, r, g, b);
  }
}

void plot_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
  uint32_t row = y * tagfb->common.framebuffer_width;
  uint32_t column = x;

  framebuffer_buffer[column + row] = (r << 16) | (g << 8) | b;
}

void draw_background_image()
{
  uint32_t x, y;
  const char *data = header_data;
  uint8_t pixel[3]; // 3 bytes per pixel (RGB)

  for (y = 0; y < 600 / 2; y++)
  {
    for (x = 0; x < 800 / 2; x++)
    {
      HEADER_PIXEL(data, pixel); // decompile
      if (x < 800 / 2 && y < 600 / 2)
      {
        plot_pixel(x * 2, y * 2, pixel[0], pixel[1], pixel[2]);
        plot_pixel(x * 2 + 1, y * 2, pixel[0], pixel[1], pixel[2]);
        plot_pixel(x * 2, y * 2 + 1, pixel[0], pixel[1], pixel[2]);
        plot_pixel(x * 2 + 1, y * 2 + 1, pixel[0], pixel[1], pixel[2]);
      }
    }
  }
}

void flip_framebuffer()
{
  uint64_t size = tagfb->common.framebuffer_width * tagfb->common.framebuffer_height;

  for (uint32_t i = 0; i < size; i += 4)
  {
    framebuffer[i] = framebuffer_buffer[i];
    framebuffer[i + 1] = framebuffer_buffer[i + 1];
    framebuffer[i + 2] = framebuffer_buffer[i + 2];
    framebuffer[i + 3] = framebuffer_buffer[i + 3];
  }
}