#ifndef API_H
#define API_H

#include "minilua.h"

#define API_TYPE_IMAGE "Image"
#define API_TYPE_FONT "Font"

void api_load_libs(lua_State *L);

#endif
