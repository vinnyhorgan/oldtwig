#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

#define ren_rgba(R, G, B, A) ((RenColor) { .r = (R), .g = (G), .b = (B), .a = (A) })

typedef struct {
    uint8_t b, g, r, a;
} RenColor;

typedef struct {
    int w, h;
    int cx, cy, cw, ch;
    RenColor *pixels;
} RenImage;

typedef struct {
    int code, x, y, w, h;
} RenGlyph;

typedef struct {
    RenImage *bitmap;
    int glyph_count;
    RenGlyph *glyphs;
} RenFont;

void ren_update(void);

RenImage* ren_create_image(int w, int h);
RenImage* ren_load_image(const char *path); // load_mem, save?
void ren_free_image(RenImage *img);

RenFont* ren_load_font(RenImage *img);
void ren_free_font(RenFont *font);
void ren_print(RenImage *dst, RenFont *font, int x, int y, const char *text, RenColor color);
int ren_text_width(RenFont *font, const char *text);
int ren_text_height(RenFont *font, const char *text);

RenColor ren_get(RenImage *img, int x, int y);
void ren_plot(RenImage *img, int x, int y, RenColor color);
void ren_clear(RenImage *img, RenColor color);
void ren_fill(RenImage *img, int x, int y, int w, int h, RenColor color);
void ren_line(RenImage *img, int x0, int y0, int x1, int y1, RenColor color);
void ren_rect(RenImage *img, int x, int y, int w, int h, RenColor color);
void ren_fill_rect(RenImage *img, int x, int y, int w, int h, RenColor color);
void ren_circle(RenImage *img, int x0, int y0, int r, RenColor color);
void ren_fill_circle(RenImage *img, int x0, int y0, int r, RenColor color);
void ren_clip(RenImage *img, int cx, int cy, int cw, int ch);
void ren_blit(RenImage *dst, RenImage *src, int dx, int dy, int sx, int sy, int w, int h);
void ren_blit_alpha(RenImage *dst, RenImage *src, int dx, int dy, int sx, int sy, int w, int h, float alpha);
void ren_blit_tint(RenImage *dst, RenImage *src, int dx, int dy, int sx, int sy, int w, int h, RenColor tint);

#endif
