#ifndef _LUA_CLASS_H_
#define _LUA_CLASS_H_

#include "lua_tinker.h"

typedef struct LuaEnum
{
	const char * name;
	const int value;
} LuaEnum;

static void luaRegEnum(lua_State * state, const LuaEnum * e)
{
	for (; e->name; ++e)
	{
		luaL_checkstack(state, 1, "enum too large");
		lua_pushinteger(state, e->value);
		lua_setglobal(state, e->name);
		//lua_setfield(state, -2, e->name);
	}
}

template< typename T >
void	RegisterMyFunc(lua_State* state, const char* func_name, lua_CFunction f)
{
	lua_tinker::push_meta(state, lua_tinker::class_name<T>::name());
	lua_pushstring(state, func_name);
	lua_pushcfunction(state, f);
	lua_settable(state, -3);
	lua_pop(state, 1);
}


int my_GetPointer(lua_State* L);

template<typename Object>
class LuaClass
{
	lua_State*  m_state;
	bool		m_bCreated;
public:
	LuaClass(lua_State* state)
	{
		m_state = state;
		m_bCreated = false;
	}

	template<typename Func>
	inline LuaClass& def(const char* name, Func func)
	{
		lua_tinker::class_def<Object>(m_state, name, func);
		return *this;
	}

	inline LuaClass& def2(const char* name, lua_CFunction func)
	{
		RegisterMyFunc<Object>(m_state, name, func);
		return *this;
	}

	inline LuaClass& class_add(const char* name)
	{
		lua_tinker::class_add<Object>(m_state, name);
		return *this;
	}

	inline LuaClass& def_CPointer(const char* name = NULL)
	{
		if (name == NULL || !name)
		{
			return def2("CPointer", my_GetPointer);
		}

		return def2(name, my_GetPointer);
	}

	inline LuaClass& create(const char* name)
	{
		if (!m_bCreated)
		{
			lua_tinker::class_add<Object>(m_state, name);
			m_bCreated = true;
		}
		lua_tinker::class_con<Object>(m_state, lua_tinker::constructor<Object>);
		return *this;
	}

	template<typename T1>
	inline LuaClass& create(const char* name)
	{
		if (!m_bCreated)
		{
			lua_tinker::class_add<Object>(m_state, name);
			m_bCreated = true;
		}
		lua_tinker::class_con<Object>(m_state, lua_tinker::constructor<Object, T1>);
		return *this;
	}
	template<typename T1, typename T2>
	inline LuaClass& create(const char* name)
	{
		if (!m_bCreated)
		{
			lua_tinker::class_add<Object>(m_state, name);
			m_bCreated = true;
		}
		lua_tinker::class_con<Object>(m_state, lua_tinker::constructor<Object, T1, T2>);
		return *this;
	}
	template<typename T1, typename T2, typename T3>
	inline LuaClass& create(const char* name)
	{
		if (!m_bCreated)
		{
			lua_tinker::class_add<Object>(m_state, name);
			m_bCreated = true;
		}
		lua_tinker::class_con<Object>(m_state, lua_tinker::constructor<Object, T1, T2, T3>);
		return *this;
	}
	template<typename T1, typename T2, typename T3, typename T4>
	inline LuaClass& create(const char* name)
	{
		if (!m_bCreated)
		{
			lua_tinker::class_add<Object>(m_state, name);
			m_bCreated = true;
		}
		lua_tinker::class_con<Object>(m_state, lua_tinker::constructor<Object, T1, T2, T3, T4>);
		return *this;
	}
	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	inline LuaClass& create(const char* name)
	{
		if (!m_bCreated)
		{
			lua_tinker::class_add<Object>(m_state, name);
			m_bCreated = true;
		}
		lua_tinker::class_con<Object>(m_state, lua_tinker::constructor<Object, T1, T2, T3, T4, T5>);
		return *this;
	}
};

#define CALL_CCLASS_USERDATA_FUNC_HEAD	\
	int top = lua_gettop( L ) ;	\
	lua_tinker::push<T>( L , userdataClass ) ;\
	lua_pushlightuserdata( L , userdataClass ) ;\
	lua_gettable( L , -2 ) ;\
	if( lua_isnil( L , -1 ) )\
{\
	lua_remove( L , -1 ) ;\
}\
	lua_pushstring( L , name ) ;\
	lua_gettable( L , -2 ) ;\
	if( ! lua_isfunction( L , -1 ) )\
{\
	lua_tinker::print_error(L, "%s attempt to call global `%s' (not a function)", __FUNCTION__ , name);\
	lua_settop( L ,top ) ;\
	return false ;\
}\
	int callproc = lua_gettop( L ) ;\
	lua_pushcclosure(L, lua_tinker::on_error, 0);\
	int errfunc = lua_gettop(L);\
	lua_pushvalue( L , callproc ) ;\



#endif