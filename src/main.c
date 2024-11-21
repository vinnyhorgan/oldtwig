#include <SDL.h>
#include "api/api.h"
#include "renderer.h"

#define WIDTH 320
#define HEIGHT 192
#define SCALE 3

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
RenImage *screen;

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");

    window = SDL_CreateWindow("Twig v0.1.0", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH * SCALE, HEIGHT * SCALE, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    screen = ren_create_image(WIDTH, HEIGHT);

    SDL_SetWindowMinimumSize(window, WIDTH, HEIGHT);
    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
    SDL_RenderSetIntegerScale(renderer, true);

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    api_load_libs(L);

    lua_newtable(L);
    for (int i = 0; i < argc; i++) {
        lua_pushstring(L, argv[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setglobal(L, "ARGS");

    lua_pushstring(L, "0.1.0");
    lua_setglobal(L, "VERSION");

    lua_pushstring(L, SDL_GetPlatform());
    lua_setglobal(L, "PLATFORM");

    luaL_dostring(L,
        "xpcall(function()\n"
        "    package.path = './data/?.lua;' .. package.path\n"
        "    package.path = './data/?/init.lua;' .. package.path\n"
        "    core = require('core')\n"
        "    core.init()\n"
        "    core.run()\n"
        "end, function(err)\n"
        "    print('Error: ' .. tostring(err))\n"
        "    print(debug.traceback(nil, 2))\n"
        "    os.exit(1)\n"
        "end)");

    lua_close(L);

    ren_free_image(screen);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
