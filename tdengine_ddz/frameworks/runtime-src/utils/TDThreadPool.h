#ifndef _TD_THREAD_POOL_H_
#define _TD_THREAD_POOL_H_

#include <vector>
#include <memory>
#include <algorithm> 
#include "TDThread.h"

class TDThreadPool
{
public:
	TDThreadPool(int nThreads);
	~TDThreadPool(void);

	size_t count();

	void join();
	void wait_for_idle();
	void terminate();

	//return the current load of this thread  
	size_t load(unsigned int n);
	//Append a task to do  
	void append(std::function< void (void) > func)  ;


protected:  
	//NO. threads  
	unsigned int m_n_threads;  
	//vector contains all the threads  
	std::vector<std::shared_ptr<TDThread> > m_vec_threads;  
};


#endif