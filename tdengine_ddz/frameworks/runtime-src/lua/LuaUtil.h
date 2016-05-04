/****************************************************************************
 ****************************************************************************/

#ifndef __LUA_UTIL_H__
#define __LUA_UTIL_H__

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace LuaUtil {
    void openLibs(lua_State* m_state);
}

#endif // __LUA_UTIL_H__
