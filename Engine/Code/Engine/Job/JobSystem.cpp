#include "JobSystem.hpp"
#include <thread>
#include <mutex>
std::mutex g_completedJobMutex;
std::mutex g_pendingJobMutex;
std::mutex g_processingJobMutex;
Job* JobSystem::FindAvailableJob()
{
    g_pendingJobMutex.lock();
    for (int i = 0; i < m_pendingJobs.size(); i++) {
        //TODO: Add thread safety check
        Job* pending = m_pendingJobs[i];
        if (pending) {
            g_processingJobMutex.lock();
            m_pendingJobs[i] = nullptr;
            m_processingJobs.push_back(pending);
            g_processingJobMutex.unlock();
            g_pendingJobMutex.unlock();
            return pending;
        }
    }
    g_pendingJobMutex.unlock();
    return nullptr;
}

std::vector<Job*> JobSystem::ClaimCompletedJobs()
{
    std::vector<Job*> result = m_completedJobs;
    m_completedJobs.clear();
    return result;
}

void JobSystem::AddPendingJob(Job* toAdd)
{
    g_pendingJobMutex.lock();
    m_pendingJobs.push_back(toAdd);
    g_pendingJobMutex.unlock();
    //Notify
    cv.notify_all();
}

void JobSystem::CompleteJob(Job* job)
{
    g_processingJobMutex.lock();
    
    for (int i = 0; i < m_processingJobs.size(); i++) {
        if (m_processingJobs[i] == job) {
            
            m_processingJobs.erase(m_processingJobs.begin() + i);
            
            g_completedJobMutex.lock();
            
            m_completedJobs.push_back(job);
            
            g_completedJobMutex.unlock();
            g_processingJobMutex.unlock();
            
            return;
        }
    }

    g_processingJobMutex.unlock();
}

JobSystem::JobSystem()
{
    int amount = ((int)std::thread::hardware_concurrency()-1);
    for (int i = 0; i < amount; i++) {
        m_jobWorkerThreads.push_back(new JobWorkerThread(this, i));
    }
}

void JobSystem::StartUp()
{

}

JobSystem::~JobSystem()
{

    bool shutDown=false;
    while (!shutDown) {
        shutDown=true;
		for (JobWorkerThread* jwt : m_jobWorkerThreads) {
            if (jwt->m_wokring) {
                shutDown=false;
                break;
            }
		}
    }

    m_jobWorkerThreads.clear();
}
