#include "TDLuaMgr.h"

#include <assert.h>
#include <string>
#include "utils/TDMacro.h"
#include "net/NetMsg.h"
#include "net/NetConfig.h"
#include "net/MessageDispatch.h"
#include "lua_tinker/lua_tinker.h"
#include "utils/TimeUtils.h"
extern "C" {
#include "lualib.h"
#include "lauxlib.h"
}


TDLuaMgr::TDLuaMgr(void)
{
	luaThreadPool = new TDThreadPool(1);
	engine = LuaEngine::getInstance();
	m_pLuaState = LuaEngine::getInstance()->getLuaStack()->getLuaState();
	//addLuaLoader(luaCustomLoader);
}


TDLuaMgr::~TDLuaMgr(void)
{
	m_mutex.lock();
	unsigned int size = m_luaElemList.size();
	for (unsigned int i = 0; i < size; i++)
	{
		delete m_luaElemList[i];
	}
	m_luaElemList.clear();
	m_mutex.unlock();
	if (luaThreadPool) {
		delete luaThreadPool;
	}
}

static TDLuaMgr* luaEngine = nullptr;
TDLuaMgr* TDLuaMgr::instance()
{
	if (luaEngine == nullptr) {
		luaEngine = new TDLuaMgr;
	}
	return luaEngine;
}

void TDLuaMgr::DestoryInstance()
{
	if (luaEngine) {
		delete luaEngine;
		luaEngine = nullptr;
	}
}


void TDLuaMgr::registerLuaGlobalVariable(const char* key, const char* value)
{
	lua_pushstring(m_pLuaState, value);
	lua_setglobal(m_pLuaState, key);
}

int TDLuaMgr::executeScriptFile(const char* filename)
{
	assert(filename != nullptr);
	lua_State* lusState = this->getLuaState();
	int nRet = luaL_dofile(lusState, filename);
	if (nRet != 0)
	{
		const char* info = lua_tostring(lusState, -1);
		cocos2d::log("[LUA ERROR] %s", lua_tostring(lusState, -1));
		lua_pop(lusState, 1);
		return nRet;
	}
	return 0;

}

void TDLuaMgr::addLuaLoader(lua_CFunction func)
{
	if (!func) return;
	// stack content after the invoking of the function
	// get loader table
	lua_getglobal(m_pLuaState, "package");                                  /* L: package */
	lua_getfield(m_pLuaState, -1, "loaders");                               /* L: package, loaders */

	// insert loader into index 2
	lua_pushcfunction(m_pLuaState, func);                                   /* L: package, loaders, func */
	int len = lua_objlen(m_pLuaState, -2) + 1;
	for (int i = lua_objlen(m_pLuaState, -2) + 1; i > 2; --i)
	{
		lua_rawgeti(m_pLuaState, -2, i - 1);                                /* L: package, loaders, func, function */
		// we call lua_rawgeti, so the loader table now is at -3
		lua_rawseti(m_pLuaState, -3, i);                                    /* L: package, loaders, func */
	}
	lua_rawseti(m_pLuaState, -2, 2);                                        /* L: package, loaders */

	// set loaders into package
	lua_setfield(m_pLuaState, -2, "loaders");                               /* L: package */

	lua_pop(m_pLuaState, 1);
}

int TDLuaMgr::executeFunction(int numArgs)
{
	lua_State* luaState = this->getLuaState();
	int functionIndex = -(numArgs + 1);
	if (!lua_isfunction(luaState, functionIndex))
	{
		cocos2d::log("value at stack [%d] is not function", functionIndex);
		lua_pop(luaState, numArgs + 1); // remove function and arguments
		return 0;
	}

	int traceback = 0;
	lua_getglobal(luaState, "__G__TRACKBACK__");                         /* L: ... func arg1 arg2 ... G */
	if (!lua_isfunction(luaState, -1))
	{
		lua_pop(luaState, 1);                                            /* L: ... func arg1 arg2 ... */
	}
	else
	{
		lua_insert(luaState, functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
		traceback = functionIndex - 1;
	}

	int error = 0;
	error = lua_pcall(luaState, numArgs, 1, traceback);                  /* L: ... [G] ret */
	if (error)
	{
		if (traceback == 0)
		{
			cocos2d::log("[LUA ERROR] %s", lua_tostring(luaState, -1));        /* L: ... error */
			lua_pop(luaState, 1); // remove error message from stack
		}
		else                                                            /* L: ... G error */
		{
			lua_pop(luaState, 2); // remove __G__TRACKBACK__ and error message from stack
		}
		return 0;
	}

	// get return value
	int ret = 0;
	if (lua_isnumber(luaState, -1))
	{
		ret = lua_tointeger(luaState, -1);
	}
	else if (lua_isboolean(luaState, -1))
	{
		ret = lua_toboolean(luaState, -1);
	}
	// remove return value from stack
	lua_pop(luaState, 1);                                                /* L: ... [G] */

	if (traceback)
	{
		lua_pop(luaState, 1); // remove __G__TRACKBACK__ from stack      /* L: ... */
	}
	return ret;
}

bool TDLuaMgr::executeLua()
{
	std::vector<LUA_ELEM *> tempList;

	m_mutex.lock();
	// 1、从队列中先取出执行的脚本函数
	// 先缓存在临时列表中，然后再执行根据临时列表中的操作，这是因为我们不希望 m_LuaElemList 被锁住太久
	unsigned int size = m_luaElemList.size();
	for (unsigned int i = 0; i < size; i++)
	{
		tempList.push_back(m_luaElemList[i]);
	}
	m_luaElemList.clear();
	m_mutex.unlock();
	if (tempList.size() == 0) {
		return false;
	}
	size = tempList.size();
	if (size > 1000)
		cocos2d::log("Lua queue has too much pending request(%d)\n", size);

	for (unsigned int i = 0; i < size; i++)
	{
		LUA_ELEM* elem = tempList[i];

		// 若该字段不为 NULL，表示为 lua string，直接执行
		if (elem->pLuaString.size() > 0)
		{
			executeString(elem->pLuaString);
		}
		else if (elem->Func != -1)
		{
			// 执行指定定义类型的函数
			switch (elem->Func)
			{
				// 收到的消息
			case FUNC_MESSAGE:

				excuteMessage(elem->fd, elem->stream);
				break;

				// 收到的数据库返回结果
			case FUNC_DB_RESULT:
				excuteDBResult(elem->cookie, elem->ret, elem->errmsg.c_str(), elem->stream);
				break;
				// 收到新连接
			case FUNC_NEW_CONNECTION:
				excuteNewConnect(elem->cookie, elem->fd);
				break;
				// 连接断开
			case FUNC_CONNECTION_LOST:
				excuteConnectLost(elem->fd);
				break;

			default:
				break;
			}
		}
		delete elem;
	}

	tempList.clear();

	return true;
}

void TDLuaMgr::applyNewConnect(int cookie, int fd)
{
	LUA_ELEM* pElem = new LUA_ELEM;
	pElem->cookie = cookie;
	pElem->fd = fd;
	pElem->Func = FUNC_NEW_CONNECTION;
	m_mutex.lock();
	m_luaElemList.push_back(pElem);
	m_mutex.unlock();
}

void TDLuaMgr::applyConnectLost(int fd)
{
	LUA_ELEM* pElem = new LUA_ELEM;
	pElem->fd = fd;
	pElem->Func = FUNC_CONNECTION_LOST;
	m_mutex.lock();
	m_luaElemList.push_back(pElem);
	m_mutex.unlock();
}

void TDLuaMgr::applyDBResult(int cookie, int ret, const char* errorString, NetMsg* input)
{
	LUA_ELEM* pElem = new LUA_ELEM;
	pElem->Func = FUNC_DB_RESULT;
	pElem->cookie = cookie;
	pElem->ret = ret;
	pElem->errmsg = errorString == nullptr ? "" : errorString;
	pElem->stream = input;
	m_mutex.lock();
	m_luaElemList.push_back(pElem);
	m_mutex.unlock();
}

void TDLuaMgr::applyMessage(int fd, NetMsg* input)
{
	LUA_ELEM* pElem = new LUA_ELEM;
	pElem->Func = FUNC_MESSAGE;
	pElem->fd = fd;
	pElem->stream = input;
	m_mutex.lock();
	m_luaElemList.push_back(pElem);
	m_mutex.unlock();
}

void TDLuaMgr::applyExcuteString(std::string pLuaString)
{
	LUA_ELEM* pElem = new LUA_ELEM;
	pElem->Func = -1;
	pElem->pLuaString = pLuaString;
	m_mutex.lock();
	m_luaElemList.push_back(pElem);
	m_mutex.unlock();
}

void TDLuaMgr::addDBOperCount()
{

}

void TDLuaMgr::excuteNewConnect(int cookie, int fd)
{
	lua_State* luaState = this->getLuaState();
	unsigned long beginTime, costTime;
	const char* message = NULL;
	int err_index = -1, para_count = -1;
	// 先压入异常处理函数
	lua_getglobal(luaState, "error_handler");

	message = "cmd_new_connection";

	// 压入脚本的入口执行函数
	lua_getglobal(luaState, message);

	// 压入第一个参数 cookie
	lua_pushnumber(luaState, cookie);

	// 压入第二个参数 fd
	lua_pushnumber(luaState, fd);

	para_count = 2;
	err_index = -4;

	
	// 记录操作执行开始时间
	beginTime = TimeUtils::getServMSecond();//TimeUtils::getServMSecond();

	// 调用函数
	int status = lua_pcall(luaState, para_count, 0, err_index);

	// 若脚本执行时间大于 100 毫秒，则记录
	costTime = TimeUtils::getServMSecond() - beginTime;
	if (costTime > 300)
		cocos2d::log("Lua execute cost too much time(%dms):%s\n", costTime, message);

	if (status)
	{
		if (!lua_isnil(luaState, -1))
		{
			cocos2d::log("%s\n", lua_tostring(luaState, -1));
		}

		lua_pop(luaState, 1);
	}

	// 弹出错误异常函数
	lua_pop(luaState, 1);
}

void TDLuaMgr::excuteConnectLost(int fd)
{
	unsigned long beginTime, costTime;
	const char* message = NULL;
	int err_index = -1, para_count = -1;
	lua_State* luaState = this->getLuaState();
	// 先压入异常处理函数
	lua_getglobal(luaState, "error_handler");

	message = "cmd_connection_lost";

	// 压入脚本的入口执行函数
	lua_getglobal(luaState, message);

	// 压入第一个参数 ret 保存的是 fd
	lua_pushnumber(luaState, fd);

	para_count = 1;
	err_index = -3;

	// 记录操作执行开始时间
	beginTime = TimeUtils::getServMSecond();//TimeUtils::getServMSecond();

	// 调用函数
	int status = lua_pcall(luaState, para_count, 0, err_index);

	// 若脚本执行时间大于 100 毫秒，则记录
	costTime = TimeUtils::getServMSecond() - beginTime;
	if (costTime > 300)
		cocos2d::log("Lua execute cost too much time(%dms):%s\n", costTime, message);

	if (status)
	{
		if (!lua_isnil(luaState, -1))
		{
			cocos2d::log("%s\n", lua_tostring(luaState, -1));
		}

		lua_pop(luaState, 1);
	}

	// 弹出错误异常函数
	lua_pop(luaState, 1);
}

void TDLuaMgr::excuteDBResult(int cookie, int ret, const char* errmsg, NetMsg* input)
{
	unsigned long beginTime, costTime;
	const char* message = NULL;
	int err_index = -1, para_count = -1;
	lua_State* luaState = this->getLuaState();
	// 先压入异常处理函数
	lua_getglobal(luaState, "error_handler");
	message = "msg_db_result";

	// 压入脚本的入口执行函数
	lua_getglobal(luaState, "msg_db_result");

	// 压入第一个参数 cookie
	lua_pushnumber(luaState, cookie);

	// 压入第二个参数 ret，成功返回 int，失败返回 string
	if (errmsg && strlen(errmsg) > 0)
	{
		// 操作失败，返回字符串
		lua_pushstring(luaState, errmsg);

		para_count = 2;
		err_index = -4;
	}
	else if (input == NULL)
	{
		// db execute 操作，返回成功
		lua_pushnumber(luaState, ret);

		para_count = 2;
		err_index = -4;
	}
	else
	{
		// db select 操作，返回结果集
		lua_pushnumber(luaState, ret);

		//// 压入第二个参数
		//lua_pushstring(m_pLuaState, message);
		// 压入第三个参数 结果集
		// 解压 packet，并将参数依次压入栈中
		input->readHead();
		std::string match = "json[]";
		MessageDispatch::instance()->unpackBuffer(luaState, input);
		para_count = 3;
		err_index = -5;
	}

	// 记录操作执行开始时间
	beginTime = TimeUtils::getServMSecond();//TimeUtils::getServMSecond();

	// 调用函数
	int status = lua_pcall(luaState, para_count, 0, err_index);

	// 若脚本执行时间大于 100 毫秒，则记录
	costTime = TimeUtils::getServMSecond() - beginTime;
	if (costTime > 300)
		cocos2d::log("Lua execute cost too much time(%dms):%s\n", costTime, message);

	if (status)
	{
		if (!lua_isnil(luaState, -1))
		{
			cocos2d::log("%s\n", lua_tostring(luaState, -1));
		}

		lua_pop(luaState, 1);
	}

	// 弹出错误异常函数
	lua_pop(luaState, 1);
}

void TDLuaMgr::excuteMessage(int fd, NetMsg* input)
{
	unsigned long beginTime, costTime;
	const char* message = NULL;
	int err_index = -1, para_count = -1;
	lua_State* luaState = this->getLuaState();
	// 先压入异常处理函数
	lua_getglobal(luaState, "error_handler");

	// 压入脚本的入口执行函数
	lua_getglobal(luaState, "global_dispatch_command");

	// 压入第一个参数（连接号）
	lua_pushnumber(luaState, fd);
	
	input->readHead();
	lua_pushstring(luaState, input->getPackName().c_str());

	lua_tinker::push<NetMsg*>(luaState, input);
	//// 压入第三个参数（消息参数）
	//// 解压 packet，并将参数依次压入栈中
	//MessageDispatch::instance()->unpackBuffer(luaState, input);

	para_count = 3;
	err_index = -5;

	// 记录操作执行开始时间
	beginTime = TimeUtils::getServMSecond();//TimeUtils::getServMSecond();

	// 调用函数
	int status = lua_pcall(luaState, para_count, 0, err_index);

	// 若脚本执行时间大于 100 毫秒，则记录
	costTime = TimeUtils::getServMSecond() - beginTime;
	if (costTime > 300)
		cocos2d::log("Lua execute cost too much time(%dms):%s\n", costTime, message);

	if (status)
	{
		if (!lua_isnil(luaState, -1))
		{
			cocos2d::log("%s\n", lua_tostring(luaState, -1));
		}

		lua_pop(luaState, 1);
	}

	// 弹出错误异常函数
	lua_pop(luaState, 1);

}

void TDLuaMgr::executeString(std::string pLuaString)
{
	unsigned long beginTime, costTime;
	int err_index = -1, para_count = -1;
	lua_State* luaState = this->getLuaState();
	// 先压入异常处理函数
	lua_getglobal(luaState, "error_handler");
	// 压入脚本的入口执行函数
	lua_getglobal(luaState, "run_string");
	// 压入第二个参数
	lua_pushstring(luaState, pLuaString.c_str());
	// 记录操作执行开始时间
	para_count = 1;
	err_index = -3;

	// 记录操作执行开始时间
	beginTime = TimeUtils::getServMSecond();//TimeUtils::getServMSecond();

	// 调用函数
	int status = lua_pcall(luaState, para_count, 0, err_index);

	// 若脚本执行时间大于 100 毫秒，则记录
	costTime = TimeUtils::getServMSecond() - beginTime;
	if (costTime > 300)
		cocos2d::log("Lua execute cost too much time(%dms):%s\n", costTime, pLuaString.c_str());

	if (status)
	{
		if (!lua_isnil(luaState, -1))
		{
			cocos2d::log("%s\n", lua_tostring(luaState, -1));
		}

		lua_pop(luaState, 1);
	}

	// 弹出错误异常函数
	lua_pop(luaState, 1);

}


int TDLuaMgr::executeFuncWithParam(std::string pFunc, std::string arg)
{
	unsigned long beginTime, costTime;
	int err_index = -1, para_count = -1;
	lua_State* luaState = this->getLuaState();
	// 先压入异常处理函数
	lua_getglobal(luaState, "error_handler");
	// 压入脚本的入口执行函数
	lua_getglobal(luaState, pFunc.c_str());
	// 压入第二个参数
	lua_pushstring(luaState, arg.c_str());
	// 记录操作执行开始时间
	para_count = 1;
	err_index = -3;

	// 记录操作执行开始时间
	beginTime = TimeUtils::getServMSecond();//TimeUtils::getServMSecond();

	// 调用函数
	int status = lua_pcall(luaState, para_count, 0, err_index);

	// 若脚本执行时间大于 100 毫秒，则记录
	costTime = TimeUtils::getServMSecond() - beginTime;
	if (costTime > 300)
		cocos2d::log("Lua execute cost too much time(%dms):%s, %s\n", costTime, pFunc.c_str(), arg.c_str());

	if (status)
	{
		if (!lua_isnil(luaState, -1))
		{
			cocos2d::log("%s\n", lua_tostring(luaState, -1));
		}

		lua_pop(luaState, 1);
		return 1;
	}

	// 弹出错误异常函数
	lua_pop(luaState, 1);
	return 0; 
}

int TDLuaMgr::executeFuncWithParam(std::string pFunc, float param1, float param2)
{
    unsigned long beginTime, costTime;
    int err_index = -1, para_count = -1;
    lua_State* luaState = this->getLuaState();
    // 先压入异常处理函数
    lua_getglobal(luaState, "error_handler");
    // 压入脚本的入口执行函数
    lua_getglobal(luaState, pFunc.c_str());
    // 压入第二个参数
    lua_pushnumber(luaState, param1);
    lua_pushnumber(luaState, param2);
    // 记录操作执行开始时间
    para_count = 2;
    err_index = -3;
    
    // 记录操作执行开始时间
    beginTime = TimeUtils::getServMSecond();//TimeUtils::getServMSecond();
    
    // 调用函数
    int status = lua_pcall(luaState, para_count, 0, err_index);
    
    // 若脚本执行时间大于 100 毫秒，则记录
    costTime = TimeUtils::getServMSecond() - beginTime;
    if (costTime > 300)
        cocos2d::log("Lua execute cost too much time(%dms):%s\n", costTime, pFunc.c_str());
    
    if (status)
    {
        if (!lua_isnil(luaState, -1))
        {
            cocos2d::log("%s\n", lua_tostring(luaState, -1));
        }
        
        lua_pop(luaState, 1);
        return 1;
    }
    
    // 弹出错误异常函数
    lua_pop(luaState, 1);
    return 0;
}

std::string TDLuaMgr::executeFuncWithParamString(std::string pFunc, std::string arg)
{
    unsigned long beginTime, costTime;
    int err_index = -1, para_count = -1;
    lua_State* luaState = this->getLuaState();
    // 先压入异常处理函数
    lua_getglobal(luaState, "error_handler");
    // 压入脚本的入口执行函数
    lua_getglobal(luaState, pFunc.c_str());
    // 压入第二个参数
    lua_pushstring(luaState, arg.c_str());
    // 记录操作执行开始时间
    para_count = 1;
    err_index = -3;
    
    // 记录操作执行开始时间
    beginTime = TimeUtils::getServMSecond();//TimeUtils::getServMSecond();
    
    // 调用函数
    int status = lua_pcall(luaState, para_count, 1, err_index);
    
    // 若脚本执行时间大于 100 毫秒，则记录
    costTime = TimeUtils::getServMSecond() - beginTime;
    if (costTime > 300)
        cocos2d::log("Lua execute cost too much time(%dms):%s\n", costTime, pFunc.c_str());
    
    if (status)
    {
        if (!lua_isnil(luaState, -1))
        {
            cocos2d::log("%s\n", lua_tostring(luaState, -1));
        }
        
        lua_pop(luaState, 1);
        return "";
    }
    
    
    
    std::string result = "";
    if(lua_isstring(luaState, -1)) {
        result =lua_tostring(luaState, -1);
    }
    
    // 弹出错误异常函数
    lua_pop(luaState, 1);
    return result;
}


std::string TDLuaMgr::convertExcuteString(std::string ori)
{
	if (ori.size() == 0) {
		return ori;
	}

	if (ori[0] == '\'') {
		std::string result = "trace(\"%o\", (";
		result.append(ori.substr(1));
		result.append("))");
		return result;
	}
	else if (ori.compare("cls") == 0) {
		system("cls");
		return "";
	}
	else if (ori.find("cmd") == 0) {
		system(ori.substr(3).c_str());
		return "";
	}
	return ori;
}

void TDLuaMgr::excuteConvertString(std::string pLuaString)
{
	executeString(convertExcuteString(pLuaString));
}
