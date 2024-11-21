#include "api.h"
#include "../renderer.h"

// currently missing all rendering functions

static RenColor checkcolor(lua_State *L, int idx) {
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

static int f_gc(lua_State *L) {
    RenImage **self = luaL_checkudata(L, 1, API_TYPE_IMAGE);
    if (*self) { ren_free_image(*self); }
    return 0;
}

static int f_create(lua_State *L) {
    int w = luaL_checknumber(L, 1);
    int h = luaL_checknumber(L, 2);
    RenImage **self = lua_newuserdata(L, sizeof(*self));
    luaL_setmetatable(L, API_TYPE_IMAGE);
    *self = ren_create_image(w, h);
    if (!*self) { luaL_error(L, "failed to create image"); }
    return 1;
}

static int f_load(lua_State *L) {
    const char *filename = luaL_checkstring(L, 1);

    char full_filename[256];
    snprintf(full_filename, sizeof(full_filename), "./data/%s", filename);

    RenImage **self = lua_newuserdata(L, sizeof(*self));
    luaL_setmetatable(L, API_TYPE_IMAGE);
    *self = ren_load_image(full_filename);
    if (!*self) { luaL_error(L, "failed to load image"); }
    return 1;
}

static int f_get_width(lua_State *L) {
    RenImage **self = luaL_checkudata(L, 1, API_TYPE_IMAGE);
    lua_pushnumber(L, (*self)->w);
    return 1;
}

static int f_get_height(lua_State *L) {
    RenImage **self = luaL_checkudata(L, 1, API_TYPE_IMAGE);
    lua_pushnumber(L, (*self)->h);
    return 1;
}

static int f_get(lua_State *L) {
    RenImage **self = luaL_checkudata(L, 1, API_TYPE_IMAGE);
    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    RenColor color = ren_get(*self, x, y);
    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);
    return 4;
}

static int f_plot(lua_State *L) {
    RenImage **self = luaL_checkudata(L, 1, API_TYPE_IMAGE);
    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    RenColor color = checkcolor(L, 4);
    ren_plot(*self, x, y, color);
    return 0;
}

static const luaL_Reg lib[] = {
    { "__gc", f_gc },
    { "create", f_create },
    { "load", f_load },
    { "get_width", f_get_width },
    { "get_height", f_get_height },
    { "get", f_get },
    { "plot", f_plot },
    { NULL, NULL }
};

int luaopen_graphics_image(lua_State *L) {
    luaL_newmetatable(L, API_TYPE_IMAGE);
    luaL_setfuncs(L, lib, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    return 1;
}
