#ifndef DRIVERS_FRAMEBUFFER_FRAMEBUFFER_H_
#define DRIVERS_FRAMEBUFFER_FRAMEBUFFER_H_

#include <types.h>

#define PSF1_FONT_MAGIC 0x0436

typedef struct
{
  uint16_t magic;        // Magic bytes for idnetiifcation.
  uint8_t fontMode;      // PSF font mode
  uint8_t characterSize; // PSF character size.
} PSF1_Header;

#define PSF_FONT_MAGIC 0x864ab572

typedef struct
{
  uint32_t magic;         /* magic bytes to identify PSF */
  uint32_t version;       /* zero */
  uint32_t headersize;    /* offset of bitmaps in file, 32 */
  uint32_t flags;         /* 0 if there's no unicode table */
  uint32_t numglyph;      /* number of glyphs */
  uint32_t bytesperglyph; /* size of each glyph */
  uint32_t height;        /* height in pixels */
  uint32_t width;         /* width in pixels */
} PSF_font;

extern void init_framebuffer();

extern void draw_text(int x, int y, const char *text, uint8_t r, uint8_t g, uint8_t b);

extern void clear_framebuffer(uint8_t r, uint8_t g, uint8_t b);
extern void plot_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
extern void flip_framebuffer();
extern void draw_vga_buffer(uint16_t *vga_buf, int width, int height);

#endif // DRIVERS_FRAMEBUFFER_FRAMEBUFFER_H_