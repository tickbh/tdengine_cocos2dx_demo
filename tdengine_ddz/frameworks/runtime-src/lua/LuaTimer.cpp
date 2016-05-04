#include "LuaTimer.h"

#include <string>
#include <map>

#include "cocos2d.h"
#include "utils/TDMacro.h"
#include "mgr/TDLuaMgr.h"
USING_NS_CC;

typedef struct _TimerEvent
{
	std::string rid;
	int cookie;
	bool isRepeat;
	int timeout;
	int timeId;
	std::string key;
} TimerEvent;

static std::map<int, TimerEvent*> allTimers;
int timer_event_del(lua_State *L);
int timer_event_set(lua_State *L);
void excuteTimeDispatch(int cookie);
int removeEvent(int cookie);

class TimerRef : public cocos2d::Node {
public:
	static TimerRef* instance() { static TimerRef value; return &value; }

	void callBack(TimerEvent* arg) {
		excuteTimeDispatch(arg->cookie);
		if (!arg->isRepeat) {
			removeEvent(arg->cookie);
		}
	}
};

int getUniqueCookie() {
	static int cookie = 0;
	cookie = (cookie + 1) % 0xffffff;
	auto iter = allTimers.find(cookie);
	while (iter != allTimers.end())
	{
		cookie = (cookie + 1) % 0xffffff;
		iter = allTimers.find(cookie);
	}
	return cookie;
}

int removeEvent(int cookie) {
	auto iter = allTimers.find(cookie);
	if(iter != allTimers.end()) {
		TimerEvent* event = iter->second;
		TimerRef::instance()->getScheduler()->unschedule(event->key, (void*)TimerRef::instance());
		allTimers.erase(iter);
	}
	return 1;
}

void excuteTimeDispatch(int cookie) {
	lua_State* L = LuaMgrIns->getLuaState();
	char info[128] = { 0 };
	sprintf(info, "timer_event_dispatch(%d)", cookie);
	LuaMgrIns->applyExcuteString(info);
}

int timer_event_del(lua_State *L) {
	if(!lua_isnumber(L, 1)) {
		cocos2d::log("error arg for timer_del");
		return 0;
	}
	
	int cookie = (int)lua_tonumber(L, 1);
	removeEvent(cookie);
	return 1;
}

int timer_event_set(lua_State *L) {
	int cookie = -1;
	if(!lua_isnumber(L, 1)) {
		cocos2d::log("error arg for timer_set for timeout");
		lua_pushnumber(L, cookie);
		return 0;
	}
	if(!lua_isboolean(L, 2)) {
		cocos2d::log("error arg for timer_set for is_repeat");
		lua_pushnumber(L, cookie);
		return 0;
	}
	cookie = getUniqueCookie();
	TimerEvent* event = new TimerEvent;
	event->isRepeat = lua_isboolean(L, 2);
	event->cookie = cookie;
	event->timeout = (int)lua_tonumber(L, 1);
	char info[250] = {};
	sprintf(info, "key_%d", cookie);
	event->key = info;
	allTimers.insert(std::pair<int, TimerEvent*>(cookie, event));

	TimerRef::instance()->getScheduler()->schedule(std::bind(&TimerRef::callBack, TimerRef::instance(), event),
		(void*)TimerRef::instance(), 1.0f * event->timeout / 1000, !event->isRepeat, event->key);
	lua_pushnumber(L, cookie);
	return 1;
}

static luaL_Reg reg_list[] = 
{
	{"timer_event_del", timer_event_del},
	{"timer_event_set", timer_event_set},
	{NULL, NULL}
};

int luaopen_lua_timer(lua_State *L)
{
	for (unsigned int i = 0; i < sizeof(reg_list) / sizeof(luaL_Reg) - 1; i++)
		lua_register(L,reg_list[i].name,reg_list[i].func);
	return 0;
}
