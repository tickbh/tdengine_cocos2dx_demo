#ifndef __LUA_REGISTER_H__
#define __LUA_REGISTER_H__

#include "td_proto_cpp/include/Proto.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace LuaRegister {
	void openLibs(lua_State* m_state);
	void pushValues(lua_State* L, td_proto::Values& values);
}

#endif
