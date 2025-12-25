#pragma once
#include <thread>
#include <condition_variable>

class JobSystem;


class JobWorkerThread {
    public:
    void ThreadMain();
    int m_threadID;
    std::thread* m_thread;
    std::condition_variable m_sleep;
    JobSystem* m_owner;
    JobWorkerThread( JobSystem* owner, int id);
    ~JobWorkerThread();
    bool m_wokring=false;
    
};