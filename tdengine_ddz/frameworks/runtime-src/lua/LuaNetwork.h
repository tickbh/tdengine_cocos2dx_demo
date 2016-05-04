#ifndef __LUA_NETWORK_H__
#define __LUA_NETWORK_H__

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace LuaNetwork {
    void openLibs(lua_State* m_state);
}

#endif
