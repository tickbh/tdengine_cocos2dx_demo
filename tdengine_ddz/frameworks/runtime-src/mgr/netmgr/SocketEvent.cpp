#include "SocketEvent.h"
#include <assert.h>

SocketEvent::SocketEvent(void)
:readInfo(new ReadInfo)
, isOnline(true)
, serverType(0)
{
}


SocketEvent::~SocketEvent(void)
{
	if (readInfo) {
		delete readInfo;
	}
}

int SocketEvent::addOuputCache(char* arg, int length)
{
	int oriSize = outputCache.size();
	outputCache.resize(outputCache.size() + length);
	memcpy(&outputCache[oriSize], arg, length);
	return outputCache.size();
}

void SocketEvent::eraseOutpuCache(int length)
{
	if (length <= 0) {
		return;
	}
	if (length == outputCache.size()) {
		outputCache.clear();
	} else {
		outputCache.erase(outputCache.begin(), outputCache.begin() + length);
	}
}
