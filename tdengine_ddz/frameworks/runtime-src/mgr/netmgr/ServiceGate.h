#ifndef _SERVICE_GATE_H_
#define _SERVICE_GATE_H_
#include "SocketEvent.h"
#include <map>
#include <mutex>

class ServiceGate
{
public:
	static ServiceGate* instance();
	static void DestoryInstance();
	~ServiceGate(void);
	void init();

	void removeSocketEvent(SocketEvent* ev);
	void newSocketEvent(int sock, int cookie);
	SocketEvent* getSocketEvent(int sock);
	void dispathMessage(int sock);
	void kickSocket(int sock);

	void onRecvAllMessage();

private:
	void initReadThread();
	void readThread();

private:
	std::map<int, SocketEvent*> cacheMaps;
	std::recursive_mutex recur_mutex;
	ServiceGate(void);
};

#endif