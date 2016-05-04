#ifndef _LUA_TIMER_H_
#define _LUA_TIMER_H_

#include "utils/TDMacro.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"


int luaopen_lua_timer (lua_State *L);
}




#endif