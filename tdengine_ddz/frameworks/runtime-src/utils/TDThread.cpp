#include "TDThread.h"

TDThread::TDThread(void)
	:m_b_is_finish(false)  
	,m_pthread(nullptr)  
{
}


TDThread::~TDThread(void)
{
	if (m_pthread != nullptr)  
		delete m_pthread;
	m_list_tasks.clear();  
}

void TDThread::join()
{
	terminate();
	if (m_pthread!=nullptr)  
		m_pthread->join();  

}

void TDThread::wait_for_idle()
{
	while(load())  {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

void TDThread::terminate()
{
	 m_b_is_finish = true;
	 notifyIncoming();
}

size_t TDThread::load()
{  
	if(m_b_is_finish) {
		return 0;
	}
	size_t sz = 0;  
	m_list_tasks_mutex.lock();  
	sz = m_list_tasks.size();  
	m_list_tasks_mutex.unlock();  
	return sz;  
}  

size_t TDThread::append(std::function< void (void) > func)  
{  
	if (m_pthread==nullptr)  
		m_pthread = new std::thread(std::bind(&TDThread::run,this));  
	size_t sz = 0;  
	m_list_tasks_mutex.lock();  
	m_list_tasks.push_back(func);  
	sz = m_list_tasks.size();  
	//if there were no tasks before, we should notidy the thread to do next job.  
	if (sz==1)  
		notifyIncoming();
	m_list_tasks_mutex.unlock();  
	return sz;  
}  

void TDThread::run()
{
	// loop wait  
	while (!m_b_is_finish)  
	{  
		std::function< void (void)> curr_task ;  
		bool bHasTasks = false;  
		m_list_tasks_mutex.lock();  
		if (m_list_tasks.empty()==false)  
		{  
			bHasTasks = true;  
			curr_task = *m_list_tasks.begin();  
		}
		m_list_tasks_mutex.unlock();  
		//doing task  
		if (bHasTasks)  
		{  
			curr_task();  
			m_list_tasks_mutex.lock();  
			m_list_tasks.pop_front();  
			m_list_tasks_mutex.unlock();  
		}
		if (!load() && !m_b_is_finish)  
		{
			std::unique_lock<std::mutex> m_cond_locker(m_cond_mutex);
			if(m_cond_incoming_task.wait_for(m_cond_locker, std::chrono::seconds(1)) == std::cv_status::timeout) {
				//LOGD("wait condition timeout");
			}
			//boost::system_time const timeout=boost::get_system_time()+ boost::posix_time::milliseconds(5000);  
			//if (m_cond_locker.mutex())  
			//	m_cond_incoming_task.timed_wait(m_cond_locker,timeout);//m_cond_incoming_task.wait(m_cond_locker);  
		}  
	}  
}

void TDThread::notifyIncoming()
{
	//m_variable_mutex.lock();
	m_cond_incoming_task.notify_one();
	imcoming_num++;
	//m_variable_mutex.unlock();
}
