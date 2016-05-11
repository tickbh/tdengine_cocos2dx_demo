/****************************************************************************
 ****************************************************************************/

#include "LuaUtil.h"


#include <string>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils/TimeUtils.h"
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

	int get_msec(lua_State* L) {
		lua_pushnumber(L, TimeUtils::getServMSecond());
		return 1;
	}


	void getRid(int serverId, const char* flag, char rid[], int len)
	{
		time_t ti;
		static char _encodeMap[33] = "0123456789ACDEFGHJKLMNPQRSTUWXYZ";
		static int _ridSequence = 0;
		static int _lastRidTime = 0;

		// Increase sequence no, it's combine to server_id to generate the RID
		_ridSequence++;
		_ridSequence &= 0x3FFFF;
		if (_ridSequence == 0)
			// Round, do carry
			_lastRidTime++;

		// Get time as 1 p
		// Get time as 1 part, if time < _lastRidTime (may be carried), use _lastRidTime
		// Notice: There may be too many rids generated in 1 second, at this time
		time(&ti);
		if (ti > _lastRidTime)
			_lastRidTime = (int)ti;
		_lastRidTime -= 1174527500; // Mar/22/2007 9:30

		/* 60bits RID
		* 00000-00000-00000-00000-00000-00000 00000-00000-00 000-00000-00000-00000
		* -------------- TIME --------------- - SERVER_ID -- --- RID SEQUENCE ----
		*/
		if (flag != NULL)
			snprintf(rid, len, "%c%c%c%c%c%c%c%c%c%c%c%c",
			flag[0],
			_encodeMap[(_lastRidTime >> 23) & 0x1F],
			_encodeMap[(_lastRidTime >> 18) & 0x1F],
			_encodeMap[(_lastRidTime >> 13) & 0x1F],
			_encodeMap[(_lastRidTime >> 8) & 0x1F],
			_encodeMap[(_lastRidTime) >> 3 & 0x1F],  //time

			_encodeMap[((_lastRidTime)& 0x7) | ((serverId >> 10) & 0x3)],   //ServerId[10..11]
			_encodeMap[(serverId >> 5) & 0x1F],        //[5..9]
			_encodeMap[(serverId)& 0x1F],       // ServerId[0..4]

			_encodeMap[(_ridSequence >> 10) & 0x1F],
			_encodeMap[(_ridSequence >> 5) & 0x1F],
			_encodeMap[(_ridSequence)& 0x1F]);
		else
			snprintf(rid, len, "%c%c%c%c%c%c%c%c%c%c%c%c",
			_encodeMap[(_lastRidTime >> 25) & 0x1F],
			_encodeMap[(_lastRidTime >> 20) & 0x1F],
			_encodeMap[(_lastRidTime >> 15) & 0x1F],
			_encodeMap[(_lastRidTime >> 10) & 0x1F],
			_encodeMap[(_lastRidTime >> 5) & 0x1F],

			_encodeMap[(_lastRidTime)& 0x1F],  // time

			_encodeMap[(serverId >> 7) & 0x1F],
			_encodeMap[(serverId >> 2) & 0x1F],       // ServerId[2..11]

			_encodeMap[((serverId << 3) & 0x18) | ((_ridSequence >> 15) & 0x7)], // ServerId[0..2] RID_SEQ[15..17]

			_encodeMap[(_ridSequence >> 10) & 0x1F],

			_encodeMap[(_ridSequence >> 5) & 0x1F],
			_encodeMap[(_ridSequence)& 0x1F]);
	}

	// 取得唯一标识ID
	int get_next_rid(lua_State *L)
	{
		char rid[12 + 1];

		// 取得 id
		int serverId = (int)luaL_checknumber(L, 1);
		const char* str = lua_tostring(L, 2);

		getRid(serverId, str, rid, 13);

		lua_pushstring(L, rid);

		return 1;
	}

    static luaL_Reg reg_list[] = 
    {
		{ "get_floder_files", get_floder_files },
		{ "get_full_path", get_full_path },
		{ "lua_print", lua_print },
		{ "get_msec", get_msec },
		{ "get_next_rid", get_next_rid },
        {NULL, NULL}
    };
    
    void openLibs(lua_State* m_state)
    {
		//luaL_setfuncs(m_state, reg_list, 3);
		for (unsigned int i = 0; i < sizeof(reg_list) / sizeof(luaL_Reg) - 1; i++)
			lua_register(m_state,reg_list[i].name,reg_list[i].func);

    }
}



