#ifndef _SOCKET_EVENT_H_
#define _SOCKET_EVENT_H_

#include <map>
#include <memory>
#include <cstdlib>
#include <vector>
#include <string>

#include "net/ReadInfo.h"
extern "C" {
#include "lua.h"
}

#define MEM_SIZE    1024

class SocketEvent
{
public:
	SocketEvent(void);
	~SocketEvent(void);

	int writeStatus;
	int connectType;
	ReadInfo* readInfo;
	bool isOnline;
	char readBuffer[MEM_SIZE];
	lua_State* state;
	inline int getSockFd() { return sockFd; }
	void setSockFd(int sockFd) { this->sockFd = sockFd;}
	std::string clientIp;
	void setServerType(int serverType) { this->serverType = serverType; }
	int serverType;
	std::vector<char> outputCache;
	int addOuputCache(char* arg, int length);
	void eraseOutpuCache(int length);
private:
	int sockFd;
};

#endif
