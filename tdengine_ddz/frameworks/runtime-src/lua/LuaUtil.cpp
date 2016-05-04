/****************************************************************************
 ****************************************************************************/

#include "LuaUtil.h"


#include <string>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils/TDUtils.h"

#ifdef WIN32
#define snprintf sprintf_s
#endif

using namespace std;

#include "cocos2d.h"
USING_NS_CC;

namespace LuaUtil {

	int get_floder_files(lua_State* L) {
		int n = lua_gettop(L);
		if(n == 0)
			return 0;

		if(!lua_isstring(L, 1))
			return 0;
		std::string floder = TDUtils::fullPathForFilename(lua_tostring(L, 1));
		std::vector<std::string> files;
		TDUtils::DfsFolder(files, floder);
		lua_newtable(L);
		for (unsigned int i = 0; i < files.size(); i++) {
			string filename = files[i];

			string::size_type pos = filename.rfind(".lua");
			if (pos == std::string::npos) {
				continue;
			}
			if (filename.size() > floder.size() && filename.find("/", floder.size() + 1) != std::string::npos) {
				continue;
			}
			filename = filename.substr(0, pos);
			lua_pushnumber(L, i + 1);
			lua_pushstring(L, filename.c_str());
			lua_settable(L, -3);
		}

		return 1;
	}

	int get_full_path(lua_State* L) {
		if (!lua_isstring(L, 1))
			return 0;
		std::string floder = TDUtils::fullPathForFilename(lua_tostring(L, 1));
		lua_pushstring(L, floder.c_str());
		return 1;
	}

	int lua_print(lua_State* L) {
		if (!lua_isstring(L, 2))
			return 0;
		cocos2d::log("%s", lua_tostring(L, 2));
		return 0;
	}

    static luaL_Reg reg_list[] = 
    {
		{ "get_floder_files", get_floder_files },
		{ "get_full_path", get_full_path },
		{ "lua_print", lua_print },
        {NULL, NULL}
    };
    
    void openLibs(lua_State* m_state)
    {
		//luaL_setfuncs(m_state, reg_list, 3);
		for (unsigned int i = 0; i < sizeof(reg_list) / sizeof(luaL_Reg) - 1; i++)
			lua_register(m_state,reg_list[i].name,reg_list[i].func);

    }
}



