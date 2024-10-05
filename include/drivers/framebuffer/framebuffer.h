#ifndef __drivers__framebuffer__framebuffer__
#define __drivers__framebuffer__framebuffer__

#include <types.h>

extern void framebuffer_clear(uint8_t r, uint8_t g, uint8_t b);
extern void plot_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
extern void framebuffer_flip();

#endif