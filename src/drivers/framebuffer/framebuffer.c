#include <drivers/framebuffer/framebuffer.h>
#include <multiboot.h>

uint32_t *framebuffer = (uint32_t *)0xffffffffbd000000;
uint32_t framebuffer_buffer[800 * 600];

extern struct multiboot_tag_framebuffer *tagfb;

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