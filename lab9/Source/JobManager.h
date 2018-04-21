#pragma once
#include "Job.h"
#include "Worker.h"
#include <list>
#include <mutex>
#include <atomic>

class JobManager {
public:
	JobManager();
	~JobManager();

	void Begin();
	void End();
	void AddJob(Job* job);
	void WaitForJobs();
	std::list<Job*>* GetJobList() { return &mJobList; }
	std::mutex* GetLock() { return &mLock; }
	std::atomic<int>* GetCounter() { return &mCounter; }

private:
	std::atomic<int> mCounter;
	bool mShutdown;
	Worker* mWorkers[4];
	std::list<Job*> mJobList;
	std::mutex mLock;
};