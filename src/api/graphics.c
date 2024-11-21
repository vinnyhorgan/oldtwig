#include "api.h"
#include "../renderer.h"

extern RenImage *screen;

static RenColor checkcolor(lua_State *L, int idx) { // make this function safer (passing 0 instead of table crashes the program)
    RenColor color;
    if (lua_isnoneornil(L, idx)) {
        return ren_rgba(0, 0, 0, 255);
    }
    lua_rawgeti(L, idx, 1);
    lua_rawgeti(L, idx, 2);
    lua_rawgeti(L, idx, 3);
    lua_rawgeti(L, idx, 4);
    color.r = luaL_checknumber(L, -4);
    color.g = luaL_checknumber(L, -3);
    color.b = luaL_checknumber(L, -2);
    color.a = luaL_optnumber(L, -1, 255);
    lua_pop(L, 4);
    return color;
}

static int f_get_width(lua_State *L) {
    lua_pushnumber(L, screen->w);
    return 1;
}

static int f_get_height(lua_State *L) {
    lua_pushnumber(L, screen->h);
    return 1;
}

static int f_update(lua_State *L) {
    ren_update();
    return 0;
}

static int f_get(lua_State *L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    RenColor color = ren_get(screen, x, y);
    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);
    return 4;
}

static int f_plot(lua_State *L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    RenColor color = checkcolor(L, 3);
    ren_plot(screen, x, y, color);
    return 0;
}

static int f_clear(lua_State *L) {
    RenColor color = checkcolor(L, 1);
    ren_clear(screen, color);
    return 0;
}

static int f_fill(lua_State *L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int w = luaL_checknumber(L, 3);
    int h = luaL_checknumber(L, 4);
    RenColor color = checkcolor(L, 5);
    ren_fill(screen, x, y, w, h, color);
    return 0;
}

static int f_line(lua_State *L) {
    int x0 = luaL_checknumber(L, 1);
    int y0 = luaL_checknumber(L, 2);
    int x1 = luaL_checknumber(L, 3);
    int y1 = luaL_checknumber(L, 4);
    RenColor color = checkcolor(L, 5);
    ren_line(screen, x0, y0, x1, y1, color);
    return 0;
}

static int f_rect(lua_State *L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int w = luaL_checknumber(L, 3);
    int h = luaL_checknumber(L, 4);
    RenColor color = checkcolor(L, 5);
    ren_rect(screen, x, y, w, h, color);
    return 0;
}

static int f_fill_rect(lua_State *L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int w = luaL_checknumber(L, 3);
    int h = luaL_checknumber(L, 4);
    RenColor color = checkcolor(L, 5);
    ren_fill_rect(screen, x, y, w, h, color);
    return 0;
}

static int f_circle(lua_State *L) {
    int x0 = luaL_checknumber(L, 1);
    int y0 = luaL_checknumber(L, 2);
    int r = luaL_checknumber(L, 3);
    RenColor color = checkcolor(L, 4);
    ren_circle(screen, x0, y0, r, color);
    return 0;
}

static int f_fill_circle(lua_State *L) {
    int x0 = luaL_checknumber(L, 1);
    int y0 = luaL_checknumber(L, 2);
    int r = luaL_checknumber(L, 3);
    RenColor color = checkcolor(L, 4);
    ren_fill_circle(screen, x0, y0, r, color);
    return 0;
}

static int f_clip(lua_State *L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int w = luaL_checknumber(L, 3);
    int h = luaL_checknumber(L, 4);
    ren_clip(screen, x, y, w, h);
    return 0;
}

static int f_blit(lua_State *L) {
    RenImage **img = luaL_checkudata(L, 1, API_TYPE_IMAGE);
    int dx = luaL_checknumber(L, 2);
    int dy = luaL_checknumber(L, 3);
    int sx = luaL_optnumber(L, 4, 0);
    int sy = luaL_optnumber(L, 5, 0);
    int w = luaL_optnumber(L, 6, (*img)->w);
    int h = luaL_optnumber(L, 7, (*img)->h);
    ren_blit(screen, *img, dx, dy, sx, sy, w, h);
    return 0;
}

static int f_blit_alpha(lua_State *L) {
    RenImage **img = luaL_checkudata(L, 1, API_TYPE_IMAGE);
    int dx = luaL_checknumber(L, 2);
    int dy = luaL_checknumber(L, 3);
    int sx = luaL_optnumber(L, 4, 0);
    int sy = luaL_optnumber(L, 5, 0);
    int w = luaL_optnumber(L, 6, (*img)->w);
    int h = luaL_optnumber(L, 7, (*img)->h);
    float alpha = luaL_optnumber(L, 8, 1.0);
    ren_blit_alpha(screen, *img, dx, dy, sx, sy, w, h, alpha);
    return 0;
}

static int f_blit_tint(lua_State *L) {
    RenImage **img = luaL_checkudata(L, 1, API_TYPE_IMAGE);
    int dx = luaL_checknumber(L, 2);
    int dy = luaL_checknumber(L, 3);
    int sx = luaL_optnumber(L, 4, 0);
    int sy = luaL_optnumber(L, 5, 0);
    int w = luaL_optnumber(L, 6, (*img)->w);
    int h = luaL_optnumber(L, 7, (*img)->h);
    RenColor tint = checkcolor(L, 8);
    ren_blit_tint(screen, *img, dx, dy, sx, sy, w, h, tint);
    return 0;
}

static int f_print(lua_State *L) {
    RenFont **font = luaL_checkudata(L, 1, API_TYPE_FONT);
    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    const char *text = luaL_checkstring(L, 4);
    RenColor color = checkcolor(L, 5);
    ren_print(screen, *font, x, y, text, color);
    return 0;
}

static const luaL_Reg lib[] = {
    { "get_width", f_get_width },
    { "get_height", f_get_height },
    { "update", f_update },
    { "get", f_get },
    { "plot", f_plot },
    { "clear", f_clear },
    { "fill", f_fill },
    { "line", f_line },
    { "rect", f_rect },
    { "fill_rect", f_fill_rect },
    { "circle", f_circle },
    { "fill_circle", f_fill_circle },
    { "clip", f_clip },
    { "blit", f_blit },
    { "blit_alpha", f_blit_alpha },
    { "blit_tint", f_blit_tint },
    { "print", f_print },
    { NULL, NULL }
};

int luaopen_graphics_image(lua_State *L);
int luaopen_graphics_font(lua_State *L);

int luaopen_graphics(lua_State *L) {
    luaL_newlib(L, lib);
    luaopen_graphics_image(L);
    lua_setfield(L, -2, "image");
    luaopen_graphics_font(L);
    lua_setfield(L, -2, "font");
    return 1;
}
