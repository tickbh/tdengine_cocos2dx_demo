#include "LuaNetwork.h"

#include <thread>

#include "mgr/netmgr/SocketEvent.h"
#include "net/NetMsg.h"
#include "net/NetConfig.h"
#include "net/MessageDispatch.h"
#include "mgr/TDLuaMgr.h"
#include "net/TDSocket.h"
#include "lua_tinker/lua_tinker.h"
#include "mgr/netmgr/ServiceGate.h"
#include "mgr/netmgr/ServiceMgr.h"

namespace LuaNetwork {
    
	// 新建连接的线程
	void thread_new_connect( string ip, unsigned short port, int timeout, int cookie ) {
		TDSocket curSocket;
		curSocket.Initialize();
		curSocket.SetNonblocking();
		curSocket.SetConnectTimeout(timeout / 1000, (timeout % 1000) * 1000);
		curSocket.SetSendBuffSize(4 * 1024 * 1024);
		curSocket.SetRecvBuffSize(4 * 1024 * 1024);
		bool success = curSocket.Open(ip.c_str(), port);
		if (success) {
			ServiceGate::instance()->newSocketEvent(curSocket.GetSocketDescriptor(), cookie);
		} else {
			LuaMgrIns->applyNewConnect(cookie, 0);
			cocos2d::log("未成功连接上远程服务端IP为%s，端口号为%d", ip.c_str(), port);
		}

	}
    // 关闭指定端口
    int close_fd(lua_State *L)
    {
        // 取得端口号
        int fd = (int)luaL_checknumber(L, 1);
		ServiceGate::instance()->kickSocket(fd);
        lua_pushnumber(L, 1);
        return 1;
    }

	int forwar_to_port(lua_State* L) {
		// 取得参数个数
		int n = lua_gettop(L);

		// 取得连接 fd
		int fd = (int)luaL_checknumber(L, 1);

		SocketEvent* ev = ServiceGate::instance()->getSocketEvent(fd);
		if (!ev)
			return 0;

		NetMsg* msg = lua_tinker::lua2object<NetMsg*>::invoke(L, 2);
		if (msg == nullptr) {
			lua_pushnil(L);
			return 1;
		}

		ServiceMgr::sendBuff(ev, (char*)msg->get(), msg->getSize());
		lua_pushnumber(L, 0);
		if (msg)
			delete msg;
		return 1;
	}

	//-1参数不合法, 0 发送成功
	int send_msg_to_port(lua_State* L) {
		// 取得参数个数
		int n = lua_gettop(L);
		// 取得连接 fd
		int fd = (int)luaL_checknumber(L, 1);
		SocketEvent* ev = ServiceGate::instance()->getSocketEvent(fd);
		if (!ev) {
			lua_pushnumber(L, -1);
			return 1;
		}

		NetMsg* msg = lua_tinker::lua2object<NetMsg*>::invoke(L, 2);
		if (msg == nullptr) {
			lua_pushnumber(L, -1);
			return 1;
		}
		bool success = ServiceMgr::sendBuff(ev, (char*)msg->get(), msg->getSize());
		lua_pushnumber(L, success ? 0 : -2);
		if (msg)
			delete msg;
		return 1;
	}
    
    int send_to_port(lua_State* L)
    {
        // 取得连接 fd
        int fd = (int)luaL_checknumber(L, 1);
		SocketEvent* ev = ServiceGate::instance()->getSocketEvent(fd);
        if (!ev)
            return 0;

		if (!lua_isstring(L, 2)) {
			return 0;
		}
		std::string msg = lua_tostring(L, 2);
		auto proto = NetConfig::instance()->getConfig().get_proto_by_name(msg);
		if (!proto) {
			return 0;
		}
		std::vector<td_proto::Values> values;
		bool success = MessageDispatch::instance()->lua_convert_value(values, L, 3, proto->args);
		if (!success)
        {
			char error_message[200];
			sprintf(error_message, "send_to_port pack message(%s) fail!\n", msg.c_str());
			luaL_argerror(L, 3, error_message);
            return 0;
        }
		NetMsg bbos;
		success = td_proto::encode_proto(bbos, NetConfig::instance()->getConfig(), msg, values);
		if (success) {
			return 0;
		}
		bbos.endSendMsg();
        // 发送数据
        // 0 表示成功、-1表示失败、-2表示缓存区阻塞
		success = ServiceMgr::sendBuff(ev, (char*)bbos.get(), bbos.getSize());
		lua_pushnumber(L, success ? 0 : -2);
        return 1;
    }
    
    // 创建新的连接
    // 参数1：ip；参数2：port；参数3：cookie
    int new_connect(lua_State* L)
    {
        if(!lua_isstring(L,1))
            return 0;
        
        // 取得 ip
        std::string strIP = lua_tostring(L,1);
        if(!lua_isnumber(L,2))
            return 0;
        
        // 取得 port
        unsigned short uPort = (unsigned short)lua_tonumber(L,2);

		if (!lua_isnumber(L, 3))
			return 0;

		// 取得 cookie
		int nTimeout = (int)lua_tonumber(L, 3);

        
        if(!lua_isnumber(L,4))
            return 0;
        
        // 取得 cookie
        int nCookies = (int)lua_tonumber(L,4);
		
		LuaMgrIns->getThreadPool()->append(std::bind(thread_new_connect, strIP, uPort, nTimeout, nCookies));
		//std::thread t(std::bind(thread_new_connect, strIP, uPort, nTimeout, nCookies));
		//t.detach();
        lua_pushnumber(L, 1);
        return 1;
    }

	// 关闭指定端口
	int close_port(lua_State *L)
	{
		// 取得端口号
		int fd = (int)luaL_checknumber(L, 1);
		ServiceGate::instance()->kickSocket(fd);
		lua_pushnumber(L, 1);
		return 1;
	}

	int pack_message(lua_State* L)
	{

		std::string msg = lua_tostring(L, 1);
		auto proto = NetConfig::instance()->getConfig().get_proto_by_name(msg);
		if (!proto) {
			return 0;
		}
		std::vector<td_proto::Values> values;
		bool success = MessageDispatch::instance()->lua_convert_value(values, L, 2, proto->args);
		if (!success)
		{
			char error_message[200];
			sprintf(error_message, "send_to_port pack message(%s) fail!\n", msg.c_str());
			luaL_argerror(L, 3, error_message);
			return 0;
		}
		NetMsg* bbos = new NetMsg;
		success = td_proto::encode_proto(*bbos, NetConfig::instance()->getConfig(), msg, values);
		if (success) {
			delete bbos;
			return 0;
		}

		lua_tinker::push<NetMsg*>(L, bbos);
		return 1;
	}


	int del_message(lua_State* L) {
		NetMsg* msg = lua_tinker::lua2object<NetMsg*>::invoke(L, 1);
		if (msg == nullptr) {
			return 0;
		}
		delete msg;
		return 0;
	}
    
    static luaL_Reg reg_list[] = 
    {
		{ "close_fd", close_fd },
		{ "send_msg_to_port", send_msg_to_port },
		{ "send_to_port", send_to_port },
		{ "new_connect", new_connect },
		{ "close_port", close_port },
		{ "forwar_to_port", forwar_to_port },
		{ "pack_message", pack_message },
		{ "del_message", del_message },
        {NULL, NULL}
    };
    
    void openLibs(lua_State* m_state)
    {
		for (unsigned int i = 0; i < sizeof(reg_list) / sizeof(luaL_Reg) - 1; i++)
			lua_register(m_state,reg_list[i].name,reg_list[i].func);
    }
}



