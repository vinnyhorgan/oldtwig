#include "api.h"
#include "../renderer.h"

static int f_gc(lua_State *L) {
    RenFont **self = luaL_checkudata(L, 1, API_TYPE_FONT);
    if (*self) { ren_free_font(*self); }
    return 0;
}

static int f_load(lua_State *L) {
    const char *filename = luaL_checkstring(L, 1);

    char full_filename[256];
    snprintf(full_filename, sizeof(full_filename), "./data/%s", filename);

    RenImage *img = ren_load_image(full_filename);
    RenFont **self = lua_newuserdata(L, sizeof(*self));
    luaL_setmetatable(L, API_TYPE_FONT);
    *self = ren_load_font(img);
    if (!*self) { luaL_error(L, "failed to load font"); }
    return 1;
}

static int f_text_width(lua_State *L) {
    RenFont **self = luaL_checkudata(L, 1, API_TYPE_FONT);
    const char *text = luaL_checkstring(L, 2);
    lua_pushnumber(L, ren_text_width(*self, text));
    return 1;
}

static int f_text_height(lua_State *L) {
    RenFont **self = luaL_checkudata(L, 1, API_TYPE_FONT);
    const char *text = luaL_checkstring(L, 2);
    lua_pushnumber(L, ren_text_height(*self, text));
    return 1;
}

static const luaL_Reg lib[] = {
    { "__gc", f_gc },
    { "load", f_load },
    { "text_width", f_text_width },
    { "text_height", f_text_height },
    { NULL, NULL }
};

int luaopen_graphics_font(lua_State *L) {
    luaL_newmetatable(L, API_TYPE_FONT);
    luaL_setfuncs(L, lib, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    return 1;
}
