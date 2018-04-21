#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

class Profiler
{
	enum { NUM_AVERAGE = 100 };

public:
	class Timer
	{
		friend class Profiler;

	public:
		void Start()
		{
			if (false == mIsStarted)
			{
				mIsStarted = true;
				mStart = std::chrono::high_resolution_clock::now();
			}
		}

		void Stop()
		{
			if (mIsStarted)
			{
				std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
				double duration = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - mStart).count();
				mCurrent_ms += 0.000001 * duration;
				if (mCurrent_ms > mMax_ms)
					mMax_ms = mCurrent_ms;
				mIsStarted = false;
			}
		}

		void Reset()
		{
			if (mCurrent_ms > 0.0)
			{
				mTimes_ms[mLastIndex] = mCurrent_ms;
				++mLastIndex;
				if (mLastIndex >= NUM_AVERAGE)
					mLastIndex = 0;
				if (mLastIndex == mFirstIndex)
				{
					++mFirstIndex;
					if (mFirstIndex >= NUM_AVERAGE)
						mFirstIndex = 0;
				}
			}
			mIsStarted = false;
			mCurrent_ms = 0.0;
		}

		const std::string &GetName() const { return mName; }
		double GetTime_ms() const { return mCurrent_ms; }
		double GetMax_ms() const { return mMax_ms; }
		double GetAvg_ms() const
		{
			double total = 0.0;
			int count = 0;
			int index = mFirstIndex; 
			while (index != mLastIndex)
			{
				total += mTimes_ms[index];
				++count;
				++index;
				if (index >= NUM_AVERAGE)
					index = 0;
			}
			if (count > 0)
				return total / count;
			return 0.0;
		}

	private:
		Timer(const std::string &name)
			: mName(name)
			, mCurrent_ms(0.0)
			, mMax_ms(0.0)
			, mIsStarted(false)
			, mFirstIndex(0)
			, mLastIndex(0)
		{
		}
		~Timer()
		{
			printf("done\n");
		}

		std::string mName;
		std::chrono::high_resolution_clock::time_point mStart;
		double mCurrent_ms;
		double mMax_ms;
		bool mIsStarted;
		double mTimes_ms[NUM_AVERAGE];
		int mFirstIndex;
		int mLastIndex;
	};

	class ScopedTimer
	{
	public:
		ScopedTimer(Timer *timer)
			: mTimer(timer)
		{
			mTimer->Start();
		}

		~ScopedTimer()
		{
			mTimer->Stop();
		}

	private:
		Timer *mTimer;
	};

	static Profiler *Get();

	Timer *GetTimer(const std::string &name)
	{
		if (mTimers.end() == mTimers.find(name))
		{
			Timer *pTimer = new Timer(name);
			mTimers[name] = pTimer;
			return pTimer;
		}
		return mTimers[name];
	}

	void ResetAll()
	{
		for (auto& iter : mTimers)
			iter.second->Reset();
	}

private:
	Profiler();
	~Profiler();
	std::unordered_map<std::string, Timer*> mTimers;
};

#define PROFILE_SCOPE(name) Profiler::ScopedTimer name_scope(Profiler::Get()->GetTimer(std::string(#name)))