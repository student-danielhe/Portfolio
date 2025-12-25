#pragma once
#include <vector>
#include <thread>
#include "Engine/Job/JobWorkerThread.hpp"
#include "Engine/Job/Job.hpp"
class JobSystem {
	public:
	    
		std::vector<JobWorkerThread*> m_jobWorkerThreads;
		std::vector<Job*> m_pendingJobs;
		std::vector<Job*> m_processingJobs;
		std::vector<Job*> m_completedJobs;
		std::mutex mtx;
		std::condition_variable cv;

		Job* FindAvailableJob();
		std::vector<Job*> ClaimCompletedJobs();
		void AddPendingJob(Job* toAdd);
		void CompleteJob(Job* job);
		JobSystem();
		void StartUp();
		~JobSystem();
};