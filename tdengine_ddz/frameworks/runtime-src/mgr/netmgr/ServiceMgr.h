#ifndef _SERVICE_MGR_H_
#define _SERVICE_MGR_H_

#include <map>
#include <memory>
#include <cstdlib>

#include "SocketEvent.h"
using namespace std;


class ServiceMgr
{
public:
	static ServiceMgr* instance();
	ServiceMgr();
	~ServiceMgr();
	void newConnect(int sock);
	void lostConnect(int sock);

	static void onWrite(SocketEvent* event);
	static bool sendBuff(int sock, char* arg, int length);
	static bool sendBuff(SocketEvent* ev, char* arg, int length);
	bool isConnect(int sock);

private:
	std::map<int, bool> connectStatus;
};

#define SerMgrIns ServiceMgr::instance()
#endif
