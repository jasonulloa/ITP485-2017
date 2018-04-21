#pragma once
#include <thread>

class JobManager;

class Worker {
public:
	Worker();
	~Worker();

	void Begin();
	void End();
	void SetShutdown(bool quit) { mShutdown = quit; }
	void SetJobManager(JobManager* manager) { mJobManager = manager; }
	static void Loop();

	static JobManager* mJobManager;
	std::thread mTask;
	static volatile bool mShutdown;
};