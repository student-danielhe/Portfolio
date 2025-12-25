#include "JobWorkerThread.hpp"
#include "Job.hpp"
#include "JobSystem.hpp"
#include <thread>
void JobWorkerThread::ThreadMain()
{
	std::unique_lock<std::mutex> lck(m_owner->mtx);
    
	m_owner->cv.wait(lck);

	while (true) {
		Job* job = nullptr;

		job = m_owner->FindAvailableJob();

		if (job) {
		    m_wokring=true;
		    job->Execute();
			std::this_thread::yield();
			m_owner->CompleteJob(job);
			m_wokring=false;
		}
		
		m_owner->cv.wait(lck);
	}
}

JobWorkerThread::JobWorkerThread(JobSystem* owner,  int id)
{
    m_owner = owner;
    m_threadID = id;
	m_thread = new std::thread(&JobWorkerThread::ThreadMain, this);
}

JobWorkerThread::~JobWorkerThread()
{
    
}

