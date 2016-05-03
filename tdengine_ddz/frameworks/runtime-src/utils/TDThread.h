#ifndef _TD_THREAD_H_
#define _TD_THREAD_H_

#include <functional>  
#include <list>  
#include <atomic>  
#include <vector>  
#include <stdlib.h>  
#include <time.h>  
#include <assert.h>  
#include <memory>  
#include <mutex>
#include <thread>
#include <condition_variable>

class TDThread
{
public:
	TDThread(void);
	~TDThread(void);

	void join();
	void wait_for_idle();
	void terminate();
	size_t load();
	size_t append(std::function< void (void) > func) ;
	void run();
protected:
	void notifyIncoming();

	std::atomic< bool>                          m_b_is_finish;          //atomic bool var to mark the thread the next loop will be terminated.  
	std::list<std::function< void (void)> >     m_list_tasks;           //The Task List contains function objects  

	int                                       imcoming_num;
	std::mutex                                m_variable_mutex;
	std::mutex                                m_list_tasks_mutex;     //The mutex with which we protect task list  
	std::thread                               *m_pthread;             //inside the thread, a task queue will be maintained.  
	std::mutex                                m_cond_mutex;           //condition mutex used by m_cond_locker  
	std::condition_variable                   m_cond_incoming_task;   //condition var with which we notify the thread for incoming tasks  

};

#endif

