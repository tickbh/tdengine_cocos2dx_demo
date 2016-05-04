#ifndef _TIME_UTILS_H_
#define _TIME_UTILS_H_

#include <chrono>
#include "TDMacro.h"

class TimeUtils
{
public:
	static u32 getServSecond()
	{
		std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
		int count = (u32)std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
		return count;
	}

	static u32 getServMSecond()
	{
		std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
		u32 count = (u32)std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
		count = count & 0x7FFFFFFF;
		return count;
	}
};

#endif

