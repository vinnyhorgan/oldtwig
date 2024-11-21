#include "renderer.h"
#include <stdlib.h>
#include <string.h>
#include <stb_image.h>

#define EXPAND(X) ((X) + ((X) > 0))

#define CLIP0(CX, X, X2, W) \
    if (X < CX) {           \
        int D = CX - X;     \
        W -= D;             \
        X2 += D;            \
        X += D;             \
    }

#define CLIP1(X, DW, W) \
    if (X + W > DW)     \
        W = DW - X;

#define CLIP()                  \
    CLIP0(dst->cx, dx, sx, w);  \
    CLIP0(dst->cy, dy, sy, h);  \
    CLIP0(0, sx, dx, w);        \
    CLIP0(0, sy, dy, h);        \
    CLIP1(dx, dst->cx + cw, w); \
    CLIP1(dy, dst->cy + ch, h); \
    CLIP1(sx, src->w, w);       \
    CLIP1(sy, src->h, h);       \
    if (w <= 0 || h <= 0)       \
    return

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;
extern RenImage *screen;

static int cp1252[] = {
    0x20ac, 0xfffd, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021, 0x02c6, 0x2030, 0x0160, 0x2039, 0x0152,
    0xfffd, 0x017d, 0xfffd, 0xfffd, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x02dc, 0x2122,
    0x0161, 0x203a, 0x0153, 0xfffd, 0x017e, 0x0178, 0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6,
    0x00a7, 0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, 0x00b0, 0x00b1, 0x00b2, 0x00b3,
    0x00b4, 0x00b5, 0x00b6, 0x00b7, 0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, 0x00c0,
    0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, 0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd,
    0x00ce, 0x00cf, 0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, 0x00d8, 0x00d9, 0x00da,
    0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, 0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
    0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, 0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4,
    0x00f5, 0x00f6, 0x00f7, 0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff,
};

static const char* decode_utf8(const char *text, int *cp) {
    unsigned char c = *text++;
    int extra = 0, min = 0;
    *cp = 0;
    if (c >= 0xf0) {
        *cp = c & 0x07;
        extra = 3;
        min = 0x10000;
    } else if (c >= 0xe0) {
        *cp = c & 0x0f;
        extra = 2;
        min = 0x800;
    } else if (c >= 0xc0) {
        *cp = c & 0x1f;
        extra = 1;
        min = 0x80;
    } else if (c >= 0x80) {
        *cp = 0xfffd;
    } else {
        *cp = c;
    }
    while (extra--) {
        c = *text++;
        if ((c & 0xc0) != 0x80) {
            *cp = 0xfffd;
            break;
        }
        (*cp) = ((*cp) << 6) | (c & 0x3f);
    }
    if (*cp < min) {
        *cp = 0xfffd;
    }
    return text;
}

static int border(RenImage *img, int x, int y) {
    RenColor top = ren_get(img, 0, 0);
    RenColor c = ren_get(img, x, y);
    return (c.r == top.r && c.g == top.g && c.b == top.b) || x >= img->w || y >= img->h;
}

static void scan(RenImage *img, int *x, int *y, int *rowh) {
    while (*y < img->h) {
        if (*x >= img->w) {
            *x = 0;
            (*y) += *rowh;
            *rowh = 1;
        }
        if (!border(img, *x, *y))
            return;
        (*x)++;
    }
}

static int load_glyphs(RenFont *font) {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    int rowh = 1;

    RenGlyph *g;
    font->glyph_count = 256 - 32;
    font->glyphs = calloc(font->glyph_count, sizeof(RenGlyph));
    if (!font->glyphs) {
        return 0;
    }

    for (int index = 0; index < font->glyph_count; index++) {
        g = &font->glyphs[index];
        scan(font->bitmap, &x, &y, &rowh);

        if (y >= font->bitmap->h) {
            return 0;
        }

        w = h = 0;
        while (!border(font->bitmap, x + w, y)) {
            w++;
        }

        while (!border(font->bitmap, x, y + h)) {
            h++;
        }

        if (index < 96) {
            g->code = index + 32;
        } else {
            g->code = cp1252[index - 96];
        }

        g->x = x;
        g->y = y;
        g->w = w;
        g->h = h;
        x += w;

        if (h != font->glyphs[0].h) {
            return 0;
        }

        if (h > rowh) {
            rowh = h;
        }
    }

    for (int i = 1; i < font->glyph_count; i++) {
        int j = i;
        RenGlyph g = font->glyphs[i];
        while (j > 0 && font->glyphs[j - 1].code > g.code) {
            font->glyphs[j] = font->glyphs[j - 1];
            j--;
        }
        font->glyphs[j] = g;
    }

    return 1;
}

static RenGlyph* get(RenFont *font, int code) {
    unsigned lo = 0, hi = font->glyph_count;
    while (lo < hi) {
        unsigned guess = (lo + hi) / 2;
        if (code < font->glyphs[guess].code)
            hi = guess;
        else
            lo = guess + 1;
    }

    if (lo == 0 || font->glyphs[lo - 1].code != code)
        return &font->glyphs['?' - 32];
    else
        return &font->glyphs[lo - 1];
}

void ren_update(void) {
    SDL_UpdateTexture(texture, NULL, screen->pixels, screen->w * sizeof(RenColor));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    static bool initial_frame = true;
    if (initial_frame) {
        SDL_ShowWindow(window);
        initial_frame = false;
    }
}

RenImage* ren_create_image(int w, int h) {
    RenImage *img = calloc(1, sizeof(RenImage));
    if (!img) return NULL;

    img->w = w;
    img->h = h;
    img->cw = -1;
    img->ch = -1;
    img->pixels = calloc(w * h, sizeof(RenColor));

    return img;
}

RenImage* ren_load_image(const char *path) {
    RenImage *img = calloc(1, sizeof(RenImage));
    if (!img) return NULL;

    img->cw = -1;
    img->ch = -1;

    img->pixels = (RenColor*) stbi_load(path, &img->w, &img->h, NULL, 4);
    if (!img->pixels) {
        free(img);
        return NULL;
    }

    uint8_t *bytes = (uint8_t*) img->pixels;
    int n = img->w * img->h * sizeof(RenColor);
    for (int i = 0; i < n; i += 4) {
        uint8_t b = bytes[i];
        bytes[i] = bytes[i + 2];
        bytes[i + 2] = b;
    }

    return img;
}

void ren_free_image(RenImage *img) {
    free(img->pixels);
    free(img);
}

RenFont* ren_load_font(RenImage *img) {
    RenFont *font = calloc(1, sizeof(RenFont));
    if (!font) return NULL;
    font->bitmap = img;
    if (!load_glyphs(font)) {
        ren_free_font(font);
        return NULL;
    }
    return font;
}

void ren_free_font(RenFont *font) {
    ren_free_image(font->bitmap);
    free(font->glyphs);
    free(font);
}

void ren_print(RenImage *dst, RenFont *font, int x, int y, const char *text, RenColor color) {
    RenGlyph *g;
    int start = x, c;

    while (*text) {
        text = decode_utf8(text, &c);
        if (c == '\r')
            continue;
        if (c == '\n') {
            x = start;
            y += ren_text_height(font, "");
            continue;
        }
        g = get(font, c);
        ren_blit_tint(dst, font->bitmap, x, y, g->x, g->y, g->w, g->h, color);
        x += g->w;
    }
}

int ren_text_width(RenFont *font, const char *text) {
    int x = 0, w = 0, c;

    while (*text) {
        text = decode_utf8(text, &c);
        if (c == '\n' || c == '\r') {
            x = 0;
        } else {
            x += get(font, c)->w;
            w = (x > w) ? x : w;
        }
    }
    return w;
}

int ren_text_height(RenFont *font, const char *text) {
    int rowh, h, c;

    h = rowh = get(font, 0)->h;
    while (*text) {
        text = decode_utf8(text, &c);
        if (c == '\n' && *text)
            h += rowh;
    }
    return h;
}

RenColor ren_get(RenImage *img, int x, int y) {
    RenColor empty = { 0, 0, 0, 0 };
    if (x >= 0 && y >= 0 && x < img->w && y < img->h)
        return img->pixels[y * img->w + x];
    return empty;
}

void ren_plot(RenImage *img, int x, int y, RenColor color) {
    int xa, i, a;

    int cx = img->cx;
    int cy = img->cy;
    int cw = img->cw >= 0 ? img->cw : img->w;
    int ch = img->ch >= 0 ? img->ch : img->h;

    if (x >= cx && y >= cy && x < cx + cw && y < cy + ch) {
        xa = EXPAND(color.a);
        a = xa * xa;
        i = y * img->w + x;

        img->pixels[i].r += (uint8_t) ((color.r - img->pixels[i].r) * a >> 16);
        img->pixels[i].g += (uint8_t) ((color.g - img->pixels[i].g) * a >> 16);
        img->pixels[i].b += (uint8_t) ((color.b - img->pixels[i].b) * a >> 16);
        img->pixels[i].a += (uint8_t) ((color.a - img->pixels[i].a) * a >> 16);
    }
}

void ren_clear(RenImage *img, RenColor color) {
    int count = img->w * img->h;
    int n;
    for (n = 0; n < count; n++)
        img->pixels[n] = color;
}

void ren_fill(RenImage *img, int x, int y, int w, int h, RenColor color) {
    RenColor *td;
    int dt, i;

    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (x + w > img->w) {
        w = img->w - x;
    }
    if (y + h > img->h) {
        h = img->h - y;
    }
    if (w <= 0 || h <= 0)
        return;

    td = &img->pixels[y * img->w + x];
    dt = img->w;
    do {
        for (i = 0; i < w; i++)
            td[i] = color;
        td += dt;
    } while (--h);
}

void ren_line(RenImage *img, int x0, int y0, int x1, int y1, RenColor color) {
    int sx, sy, dx, dy, err, e2;
    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    if (x0 < x1)
        sx = 1;
    else
        sx = -1;
    if (y0 < y1)
        sy = 1;
    else
        sy = -1;
    err = dx - dy;

    do {
        ren_plot(img, x0, y0, color);
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    } while (x0 != x1 || y0 != y1);
}

void ren_rect(RenImage *img, int x, int y, int w, int h, RenColor color) {
    int x1, y1;
    if (w <= 0 || h <= 0) {
        return;
    }

    if (w == 1) {
        ren_line(img, x, y, x, y + h, color);
    }
    else if (h == 1) {
        ren_line(img, x, y, x + w, y, color);
    }
    else {
        x1 = x + w - 1;
        y1 = y + h - 1;
        ren_line(img, x, y, x1, y, color);
        ren_line(img, x1, y, x1, y1, color);
        ren_line(img, x1, y1, x, y1, color);
        ren_line(img, x, y1, x, y, color);
    }
}

void ren_fill_rect(RenImage *img, int x, int y, int w, int h, RenColor color) {
    x += 1;
    y += 1;
    w -= 2;
    h -= 2;

    int cx = img->cx;
    int cy = img->cy;
    int cw = img->cw >= 0 ? img->cw : img->w;
    int ch = img->ch >= 0 ? img->ch : img->h;

    if (x < cx) {
        w += (x - cx);
        x = cx;
    }
    if (y < cy) {
        h += (y - cy);
        y = cy;
    }
    if (x + w > cx + cw) {
        w -= (x + w) - (cx + cw);
    }
    if (y + h > cy + ch) {
        h -= (y + h) - (cy + ch);
    }
    if (w <= 0 || h <= 0)
        return;

    RenColor *td = &img->pixels[y * img->w + x];
    int dt = img->w;
    int xa = EXPAND(color.a);
    int a = xa * xa;

    do {
        for (int i = 0; i < w; i++) {
            td[i].r += (uint8_t) ((color.r - td[i].r) * a >> 16);
            td[i].g += (uint8_t) ((color.g - td[i].g) * a >> 16);
            td[i].b += (uint8_t) ((color.b - td[i].b) * a >> 16);
            td[i].a += (uint8_t) ((color.a - td[i].a) * a >> 16);
        }
        td += dt;
    } while (--h);
}

void ren_circle(RenImage *img, int x0, int y0, int r, RenColor color) {
    int E = 1 - r;
    int dx = 0;
    int dy = -2 * r;
    int x = 0;
    int y = r;

    ren_plot(img, x0, y0 + r, color);
    ren_plot(img, x0, y0 - r, color);
    ren_plot(img, x0 + r, y0, color);
    ren_plot(img, x0 - r, y0, color);

    while (x < y - 1) {
        x++;

        if (E >= 0) {
            y--;
            dy += 2;
            E += dy;
        }

        dx += 2;
        E += dx + 1;

        ren_plot(img, x0 + x, y0 + y, color);
        ren_plot(img, x0 - x, y0 + y, color);
        ren_plot(img, x0 + x, y0 - y, color);
        ren_plot(img, x0 - x, y0 - y, color);

        if (x != y) {
            ren_plot(img, x0 + y, y0 + x, color);
            ren_plot(img, x0 - y, y0 + x, color);
            ren_plot(img, x0 + y, y0 - x, color);
            ren_plot(img, x0 - y, y0 - x, color);
        }
    }
}

void ren_fill_circle(RenImage *img, int x0, int y0, int r, RenColor color) {
    if (r <= 0) {
        return;
    }

    int E = 1 - r;
    int dx = 0;
    int dy = -2 * r;
    int x = 0;
    int y = r;

    ren_line(img, x0 - r + 1, y0, x0 + r, y0, color);

    while (x < y - 1) {
        x++;

        if (E >= 0) {
            y--;
            dy += 2;
            E += dy;
            ren_line(img, x0 - x + 1, y0 + y, x0 + x, y0 + y, color);
            ren_line(img, x0 - x + 1, y0 - y, x0 + x, y0 - y, color);
        }

        dx += 2;
        E += dx + 1;

        if (x != y) {
            ren_line(img, x0 - y + 1, y0 + x, x0 + y, y0 + x, color);
            ren_line(img, x0 - y + 1, y0 - x, x0 + y, y0 - x, color);
        }
    }
}

void ren_clip(RenImage *img, int cx, int cy, int cw, int ch) {
    img->cx = cx;
    img->cy = cy;
    img->cw = cw;
    img->ch = ch;
}

void ren_blit(RenImage *dst, RenImage *src, int dx, int dy, int sx, int sy, int w, int h) {
    int cw = dst->cw >= 0 ? dst->cw : dst->w;
    int ch = dst->ch >= 0 ? dst->ch : dst->h;

    CLIP();

    RenColor *ts = &src->pixels[sy * src->w + sx];
    RenColor *td = &dst->pixels[dy * dst->w + dx];
    int st = src->w;
    int dt = dst->w;
    do {
        memcpy(td, ts, w * sizeof(RenColor));
        ts += st;
        td += dt;
    } while (--h);
}

void ren_blit_alpha(RenImage *dst, RenImage *src, int dx, int dy, int sx, int sy, int w, int h, float alpha) {
    alpha = (alpha < 0) ? 0 : (alpha > 1 ? 1 : alpha);
    ren_blit_tint(dst, src, dx, dy, sx, sy, w, h, ren_rgba(0xff, 0xff, 0xff, (uint8_t) (alpha * 255)));
}

void ren_blit_tint(RenImage *dst, RenImage *src, int dx, int dy, int sx, int sy, int w, int h, RenColor tint) {
    int cw = dst->cw >= 0 ? dst->cw : dst->w;
    int ch = dst->ch >= 0 ? dst->ch : dst->h;

    CLIP();

    int xr = EXPAND(tint.r);
    int xg = EXPAND(tint.g);
    int xb = EXPAND(tint.b);
    int xa = EXPAND(tint.a);

    RenColor *ts = &src->pixels[sy * src->w + sx];
    RenColor *td = &dst->pixels[dy * dst->w + dx];
    int st = src->w;
    int dt = dst->w;
    do {
        for (int x = 0; x < w; x++) {
            unsigned r = (xr * ts[x].r) >> 8;
            unsigned g = (xg * ts[x].g) >> 8;
            unsigned b = (xb * ts[x].b) >> 8;
            unsigned a = xa * EXPAND(ts[x].a);
            td[x].r += (uint8_t) ((r - td[x].r) * a >> 16);
            td[x].g += (uint8_t) ((g - td[x].g) * a >> 16);
            td[x].b += (uint8_t) ((b - td[x].b) * a >> 16);
            td[x].a += (uint8_t) ((ts[x].a - td[x].a) * a >> 16);
        }
        ts += st;
        td += dt;
    } while (--h);
}
