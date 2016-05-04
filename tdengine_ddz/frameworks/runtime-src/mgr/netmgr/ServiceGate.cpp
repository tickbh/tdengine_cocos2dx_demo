#include "ServiceGate.h"
#include <assert.h>
#include <thread>
#include <condition_variable>

#include "cocos2d.h"

#include "mgr/TDLuaMgr.h"
#include "net/TDSocket.h"
#include "net/NetMsg.h"
#include "SocketEvent.h"
#include "ServiceMgr.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

static ServiceGate* serviceGate = nullptr;
ServiceGate* ServiceGate::instance()
{
	if(serviceGate == nullptr) {
		serviceGate = new ServiceGate;
		//serviceGate->initReadThread();
	}
	return serviceGate;
}

void ServiceGate::DestoryInstance()
{
	if (serviceGate) {
		delete serviceGate;
		serviceGate = nullptr;
	}
}


ServiceGate::ServiceGate(void)
{
}

ServiceGate::~ServiceGate(void)
{
	std::lock_guard<std::recursive_mutex> lock(recur_mutex);
	for (auto iter : cacheMaps)
	{
		delete iter.second;
	}
}

void ServiceGate::init()
{

}

void ServiceGate::removeSocketEvent( SocketEvent* ev )
{
	assert(ev != nullptr);
	std::lock_guard<std::recursive_mutex> lock(recur_mutex);
	auto evit = cacheMaps.find(ev->getSockFd());
	if(evit == cacheMaps.end()) {
		cocos2d::log("already kick %d", ev->getSockFd());
		return;
	}
	cacheMaps.erase(evit);
	LuaMgrIns->applyConnectLost(ev->getSockFd());
	TDSocket curSocket(ev->getSockFd());
	curSocket.Close();
	delete ev;
	ev = nullptr;
	cocos2d::log("now there is %d client", cacheMaps.size());
}

SocketEvent* ServiceGate::getSocketEvent( int sock )
{
	std::lock_guard<std::recursive_mutex> lock(recur_mutex);
	auto ev = cacheMaps.find(sock);
	if(ev != cacheMaps.end()) {
		return ev->second;
	}
	return NULL;
}

void ServiceGate::newSocketEvent(int  sock, int cookie)
{
	std::lock_guard<std::recursive_mutex> lock(recur_mutex);
	SocketEvent* ev = new SocketEvent;
	ev->setSockFd(sock);
	LuaMgrIns->applyNewConnect(cookie, ev->getSockFd());
	cacheMaps.insert(std::pair<int, SocketEvent*>(ev->getSockFd(), ev));
	cocos2d::log("now there is %d client", cacheMaps.size());
}

void ServiceGate::dispathMessage( int sock )
{
	SocketEvent* ev = getSocketEvent(sock);
	int nextSize = 0;
	char* message = ev->readInfo->getNextMessage(nextSize);
	while (message != nullptr)
	{
		NetMsg* input = new NetMsg(message, nextSize);
		LuaMgrIns->excuteMessage(sock, input);
		message = ev->readInfo->getNextMessage(nextSize);
	}
}

void ServiceGate::kickSocket( int sock )
{
	std::lock_guard<std::recursive_mutex> lock(recur_mutex);
	auto evit = cacheMaps.find(sock);
	if(evit == cacheMaps.end()) {
		return;
	}
	cacheMaps.erase(evit);
	TDSocket curSocket(sock);
	curSocket.Close();
}

void ServiceGate::onRecvAllMessage()
{
	std::map<int, SocketEvent*>::iterator it;
	for (it = cacheMaps.begin(); it != cacheMaps.end(); ++it)
	{
		SocketEvent* pSockEvt = it->second;
		TDSocket curSocket(pSockEvt->getSockFd());
		bool needDispatch = false;
		int size = 0;
		do 
		{
			//int time = TimeUtils::getServMSecond();
			size = curSocket.Receive(1024);
			//cocos2d::log("recv msg time is %d", TimeUtils::getServMSecond() - time);
			int err = 0;
			if (size == -1) {
				if (errno == EAGAIN || (errno == EINTR) || (errno == ENOENT)) {
					/* Try again later */
				}
				else {
					err = -1;
				}
			}
			else if (size == 0) {
				err = -1;
			}

			//资源未准备完毕，等待
			if (curSocket.GetSocketError() == TDSocket::SocketEwouldblock) {
				err = 0;
			}

			if (err == -1) {
				cocos2d::log("error num is %d socket error is %d socket fd = %d", errno, curSocket.GetSocketError(), pSockEvt->getSockFd());
				ServiceGate::removeSocketEvent(pSockEvt);
				return;
			}
			if (size > 0)
			{
				pSockEvt->readInfo->appendBuffer((char*)curSocket.GetData(), curSocket.GetBytesReceived());
				needDispatch = true;
			}
		} while (size == 1024);
		
		if (needDispatch)
		{
			ServiceGate::dispathMessage(pSockEvt->getSockFd());
		}
	}
}

