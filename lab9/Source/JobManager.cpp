#include "ITPEnginePCH.h"
#include "JobManager.h"

volatile bool Worker::mShutdown;

JobManager::JobManager() {
	for (int i = 0; i < 4; i++) {
		mWorkers[i] = new Worker();
		mWorkers[i]->SetJobManager(this);
	}
	mShutdown = false;
	mCounter = 0;
}

JobManager::~JobManager() {
	End();

	for (int i = 0; i < 4; i++) {
		delete mWorkers[i];
	}
}

void JobManager::Begin() {
	for (int i = 0; i < 4; i++) {
		mWorkers[i]->Begin();
	}
}

void JobManager::End() {
	mShutdown = true;

	for (int i = 0; i < 4; i++) {
		mWorkers[i]->SetShutdown(mShutdown);
		mWorkers[i]->End();
	}
}

void JobManager::AddJob(Job* job) {
	mLock.lock();
	mJobList.push_back(job);
	mCounter++;
	mLock.unlock();
}

void JobManager::WaitForJobs() {
	while (mCounter > 0) {
		Sleep(1);
	}
}