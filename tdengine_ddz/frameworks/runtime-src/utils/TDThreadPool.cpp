#include "TDThreadPool.h"


TDThreadPool::TDThreadPool(int nThreads)
	:m_n_threads(nThreads)
{
	assert(nThreads>0 && nThreads<=512);  
	for (int i = 0; i< nThreads ;i++)  
		m_vec_threads.push_back(std::shared_ptr<TDThread>(new TDThread()));  
}


TDThreadPool::~TDThreadPool(void)
{
	this->join();
}

size_t TDThreadPool::count()
{
	return m_vec_threads.size();
}  

void TDThreadPool::join()
{

	for_each(m_vec_threads.begin(), m_vec_threads.end(),[this](std::shared_ptr<TDThread> & item)  
	{
		item->join();
	});  
}  

void TDThreadPool::wait_for_idle()
{  
	int n_tasks = 0;  
	do  
	{
		//if (n_tasks)  
		//	boost::this_thread::sleep(boost::posix_time::milliseconds(200));  
		n_tasks = 0;  
		for_each(m_vec_threads.begin(),m_vec_threads.end(),[this,&n_tasks](std::shared_ptr<TDThread> & item)  
		{  
			n_tasks += item->load();  
		});  
	}while (n_tasks);  

}  

void TDThreadPool::terminate()
{  
	for_each(m_vec_threads.begin(),m_vec_threads.end(),[this](std::shared_ptr<TDThread> & item)  
	{  
		item->terminate();  
	});  
}  

size_t TDThreadPool::load( unsigned int n )

{  
	return (n>=m_vec_threads.size())?0:m_vec_threads[n]->load();  
}  

void TDThreadPool::append( std::function< void (void) > func )
{  
	unsigned int nIdx = -1;  
	unsigned int nMinLoad = -1;  
	for (unsigned int i=0;i<m_n_threads;i++)  
	{  
		if (nMinLoad > m_vec_threads[i]->load())  
		{  
			nMinLoad = m_vec_threads[i]->load();  
			nIdx = i;  
		}  
	}  
	
	assert(nIdx>=0 && nIdx<m_n_threads);  
	m_vec_threads[nIdx]->append(func);  
}  
