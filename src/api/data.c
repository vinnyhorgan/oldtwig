#include "api.h"
#include <LzmaLib.h>
#include <stdlib.h>

static int f_compress(lua_State *L) {
    size_t src_len;
    const char *src = luaL_checklstring(L, 1, &src_len);

    size_t dst_len = src_len + (src_len >> 3) + 16384;
    unsigned char *dst = malloc(dst_len);
    if (!dst) { luaL_error(L, "out of memory"); }

    size_t props_len = LZMA_PROPS_SIZE;
    size_t max_dst_len = dst_len - 4 - LZMA_PROPS_SIZE;

    int r = LzmaCompress(dst + 4 + LZMA_PROPS_SIZE, &max_dst_len, src, src_len, dst + 4, &props_len, 5, (1 << 24), 3, 0, 2, 32, 1);
    if (r != SZ_OK) {
        free(dst);
        lua_pushnil(L);
        return 1;
    }

    dst[0] = src_len & 0xff;
    dst[1] = (src_len >> 8) & 0xff;
    dst[2] = (src_len >> 16) & 0xff;
    dst[3] = (src_len >> 24) & 0xff;

    lua_pushlstring(L, dst, max_dst_len + 4 + LZMA_PROPS_SIZE);
    free(dst);
    return 1;
}

static int f_uncompress(lua_State *L) {
    size_t src_len;
    const char *src = luaL_checklstring(L, 1, &src_len);

    size_t dst_len = (src[0] & 0xff) | ((src[1] & 0xff) << 8) | ((src[2] & 0xff) << 16) | ((src[3] & 0xff) << 24);
    unsigned char *dst = malloc(dst_len + 8);
    if (!dst) { luaL_error(L, "out of memory"); }

    src_len = src_len - 4 - LZMA_PROPS_SIZE;

    int r = LzmaUncompress(dst, &dst_len, src + 4 + LZMA_PROPS_SIZE, &src_len, src + 4, LZMA_PROPS_SIZE);
    if (r != SZ_OK) {
        free(dst);
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(L, dst, dst_len);
    free(dst);
    return 1;
}

static const luaL_Reg lib[] = {
    { "compress", f_compress },
    { "uncompress", f_uncompress },
    { NULL, NULL }
};

int luaopen_data(lua_State *L) {
    luaL_newlib(L, lib);
    return 1;
}
