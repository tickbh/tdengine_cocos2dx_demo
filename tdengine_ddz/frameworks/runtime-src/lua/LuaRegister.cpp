#include "LuaRegister.h"
#include "lua_tinker/lua_tinker.h"


namespace lua_tinker {

	template<>
	void push(lua_State *L, td_proto::Values& val)
	{
		switch (val.sub_type)
		{
		case td_proto::TYPE_NIL:
			lua_pushnil(L);
			break;
		case td_proto::TYPE_U8:
			lua_pushinteger(L, val._u8);
			break;
		case td_proto::TYPE_I8:
			lua_pushinteger(L, val._i8);
			break;
		case td_proto::TYPE_U16:
			lua_pushinteger(L, val._u16);
			break;
		case td_proto::TYPE_I16:
			lua_pushinteger(L, val._i16);
			break;
		case td_proto::TYPE_U32:
			lua_pushinteger(L, val._u32);
			break;
		case td_proto::TYPE_I32:
			lua_pushinteger(L, val._i32);
			break;
		case td_proto::TYPE_FLOAT:
			lua_pushnumber(L, val._f);
			break;
		case td_proto::TYPE_STR:
			lua_pushstring(L, val._str->c_str());
			break;
		case td_proto::TYPE_RAW:
			lua_pushlstring(L, &(*val._raw)[0], val._raw->size());
			break;
		case td_proto::TYPE_MAP:
			lua_newtable(L);
			for (auto iter : *val._map) {
				lua_pushstring(L, iter.first.c_str());
				lua_tinker::push(L, iter.second);
				lua_settable(L, -3);
			}
			break;
		case td_proto::TYPE_AU8:
		case td_proto::TYPE_AI8:
		case td_proto::TYPE_AU16:
		case td_proto::TYPE_AU32:
		case td_proto::TYPE_AI32:
		case td_proto::TYPE_AFLOAT:
		case td_proto::TYPE_ASTR:
		case td_proto::TYPE_ARAW:
		case td_proto::TYPE_AMAP:
		{
			lua_newtable(L);
			int i = 1;
			for (auto iter : *val._array) {
				lua_pushnumber(L, i++);
				lua_tinker::push(L, iter);
				lua_settable(L, -3);
			}
			break;
		}
		default:
			break;
		}
	}
}

void LuaRegister::openLibs(lua_State* m_state)
{

}

void LuaRegister::pushValues(lua_State* L, td_proto::Values& values)
{
	lua_tinker::push(L, values);
}
