#include <drivers/framebuffer/framebuffer.h>
#include <multiboot.h>

PSF_font *font;

uint32_t *framebuffer = (uint32_t *)0xffffffffbd000000;
uint32_t framebuffer_buffer[800 * 600];

extern struct multiboot_tag_framebuffer *tagfb;
extern char _binary_font_psf_start;
int font_bytesPerLine;

void framebuffer_init()
{
  font = (PSF_font *)&_binary_font_psf_start;
  font_bytesPerLine = (font->width + 7) / 8;
}

void framebuffer_clear(uint8_t r, uint8_t g, uint8_t b)
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

void draw_text(int cx, int cy, const char *text, uint8_t r, uint8_t g, uint8_t b)
{
  int len = strlen(text);

  for (int i = 0; i < len; i++)
  {
    uint8_t ch = text[i];
    uint8_t *glyph = (uint8_t *)&_binary_font_psf_start + font->headersize + (ch > 0 && ch < font->numglyph ? ch : 0) * font->bytesperglyph;

    int mask;
    for (uint32_t y = 0; y < font->height; y++)
    {
      mask = 1 << (font->width - 1);
      for (uint32_t x = 0; x < font->width; x++)
      {
        if (*((unsigned int *)glyph) & mask)
        {
          plot_pixel(cx + (i * font->width) + x, cy + y, r, g, b);
        }

        mask >>= 1;
      }

      glyph += font_bytesPerLine;
    }
  }
}

void videobuffer_draw_vga_buffer(uint16_t *vga_buf, int width, int height)
{
  uint8_t r = 128;
  uint8_t g = 128;
  uint8_t b = 128;

  for (int py = 0; py < height; py++)
  {

    char tempText[width + 1];
    for (int px = 0; px < width + 1; px++)
      tempText[width] = 0;

    for (int px = 0; px < width; px++)
    {
      tempText[px] = vga_buf[px + (py * width)];
    }
    draw_text(0, py * 16 + 16, tempText, r * 1.2, g * 1.2, b * 1.2);
  }
}

void plot_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
  uint32_t row = y * tagfb->common.framebuffer_width;
  uint32_t column = x;

  framebuffer_buffer[column + row] = (r << 16) | (g << 8) | b;
}

void framebuffer_flip()
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