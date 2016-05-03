#ifndef __TD_LUA_MGR_H__
#define __TD_LUA_MGR_H__

extern "C" {
#include "lua.h"
extern int luaCustomLoader(lua_State *L);
}

#include "cocos2d.h"
#include <mutex>
#include <vector>
#include <list>
#include "utils/TDThreadPool.h"
#include "CCLuaEngine.h"

USING_NS_CC;

#define LUA_FUNC_INIT(p) { #p, (lua_CFunction)p }
#define LUA_RETURN(r) { lua_pushnumber(L, r); return 1; }

#define FUNC_MESSAGE         1
#define FUNC_DB_RESULT       2
#define FUNC_NEW_CONNECTION  3
#define FUNC_CONNECTION_LOST 4
#define FUNC_PING_LOST       5

class NetMsg;

typedef struct luaElem
{
	std::string pLuaString;
	int Func;
	int fd;
	NetMsg* stream;
	std::string errmsg;
	int cookie;
	int ret;
} LUA_ELEM;

class TDLuaMgr
{
public:
	static TDLuaMgr* instance();
	static void DestoryInstance();
	TDLuaMgr(void);
	~TDLuaMgr(void);

	LuaEngine* getLuaEngine() { return LuaEngine::getInstance(); }

	lua_State* getLuaState(void) { return getLuaEngine()->getLuaStack()->getLuaState(); }
	TDThreadPool* getThreadPool() { return luaThreadPool; }
	
	void registerLuaGlobalVariable(const char* key, const char* value);
	void addLuaLoader(lua_CFunction func);
	int executeScriptFile(const char* filename);
	int executeFunction(int numArgs);
	bool executeLua();
	void applyNewConnect(int cookie, int fd);
	void applyConnectLost(int fd);
	void applyDBResult(int cookie, int ret, const char* errorString, NetMsg* input);
	void applyMessage(int fd, NetMsg* input);
	void applyExcuteString(std::string pLuaString);

	void excuteNewConnect(int cookie, int fd);
	void excuteConnectLost(int fd);
	void excuteDBResult(int cookie, int ret, const char* errmsg, NetMsg* input);
	void excuteMessage(int fd, NetMsg* input);
	void executeString(std::string pLuaString);
	int executeFuncWithParam(std::string pFunc, std::string arg);
    int executeFuncWithParam(std::string pFunc, float param1, float param2);
    std::string executeFuncWithParamString(std::string pFunc, std::string arg);

	void excuteConvertString(std::string pLuaString);

	void addDBOperCount();
	std::string convertExcuteString(std::string ori);
private:
	std::mutex m_stateLock;
	std::mutex m_mutex;
	lua_State* m_pLuaState;
	std::vector< LUA_ELEM* > m_luaElemList;
	std::list<lua_State*> m_lstFreeState;
	TDThreadPool* luaThreadPool;
	LuaEngine* engine;
};

#define LuaMgrIns TDLuaMgr::instance()
#define LuaEngIns LuaEngine::getInstance()
#endif // __CC_LUA_ENGINE_H__
