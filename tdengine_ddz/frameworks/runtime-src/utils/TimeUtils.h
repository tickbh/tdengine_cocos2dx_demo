#ifndef _TIME_UTILS_H_
#define _TIME_UTILS_H_

#include <chrono>
#include "TDMacro.h"

class TimeUtils
{
public:
	static int getServSecond()
	{
		std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
		int count = (uint32)std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
		return count;
	}

	static int getServMSecond()
	{
		std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
		uint32 count = (uint32)std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
		count = count & 0x7FFFFFFF;
		return count;
	}
};

#endif

