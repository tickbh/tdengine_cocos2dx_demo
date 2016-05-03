#include "ServiceMgr.h"
#include "SocketEvent.h"

#include "ServiceGate.h"

#include "net/TDSocket.h"
#include <errno.h>

ServiceMgr::ServiceMgr()
{
}

ServiceMgr::~ServiceMgr()
{
}

ServiceMgr* ServiceMgr::instance()
{
	static ServiceMgr s_mgr;
	return &s_mgr;
}

void ServiceMgr::onWrite(SocketEvent* event)
{

}


bool ServiceMgr::sendBuff(int sock, char* arg, int length)
{
	SocketEvent* ev = ServiceGate::instance()->getSocketEvent(sock);
	return sendBuff(ev, arg, length);
}

bool ServiceMgr::sendBuff(SocketEvent* ev, char* arg, int length)
{
	TDSocket curSocket(ev->getSockFd());
	if (ev->outputCache.size() == 0) {
		int size = curSocket.Send(arg, length);
		if (size != length) {
			ev->addOuputCache(arg + size, length - size);
			return false;
		}
	}
	else {
		int resultSize = ev->addOuputCache(arg, length);
		int size = curSocket.Send(&ev->outputCache[0], ev->outputCache.size());
		ev->eraseOutpuCache(size);
		if (size != resultSize) {
			return false;
		}
	}
	return true;
}

void ServiceMgr::newConnect(int sock)
{
	connectStatus.insert(make_pair(sock, true));
}

void ServiceMgr::lostConnect(int sock)
{
	connectStatus.erase(sock);
}

bool ServiceMgr::isConnect(int sock)
{
	auto iter = connectStatus.find(sock);
	return iter != connectStatus.end();
}