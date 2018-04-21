#include "ITPEnginePCH.h"
#include "Worker.h"

JobManager* Worker::mJobManager;

Worker::Worker() {
	mShutdown = false;
}

Worker::~Worker() {

}

void Worker::Begin() {
	mTask = std::thread(Loop);
}

void Worker::End() {
	mTask.join();
}

void Worker::Loop() {
	while (!mShutdown) {
		std::mutex* lock = mJobManager->GetLock();
		std::list<Job*> *list = mJobManager->GetJobList();
		std::atomic<int>* count = mJobManager->GetCounter();
		Job* task = nullptr;

		lock->lock();
		if (!list->empty()) {
			task = list->front();
			list->pop_front();
			*(count) -= 1;
		}
		lock->unlock();

		if (task != nullptr) {
			task->DoIt();
			delete task;
		}
	}
}