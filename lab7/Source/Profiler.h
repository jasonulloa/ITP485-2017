#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

class Profiler
{
	enum { NUM_AVERAGE = 100 };

public:
	static FILE *mFile;
	static bool doComma;
	//------------------------------------------------------------------------------------
	// A simple stopwatch type of timer
	//------------------------------------------------------------------------------------
	class Timer
	{
		friend class Profiler;

	public:
		/// Start the timer
		void Start()
		{
			if (false == mIsStarted)
			{
				mIsStarted = true;
				mStart = std::chrono::high_resolution_clock::now();
				uint64_t starttime = std::chrono::duration_cast<std::chrono::nanoseconds>(mStart.time_since_epoch()).count();
				if (Profiler::doComma) {
					fprintf(Profiler::mFile, ",\n");
				}
				fprintf(Profiler::mFile, "{\"name\": \"%s\", \"ph\": \"B\", \"ts\": %llu, \"pid\": 1, \"tid\": 1},\n", GetName().c_str(), starttime);
				Profiler::doComma = false;
			}
		}

		/// Stop the timer
		void Stop()
		{
			if (mIsStarted)
			{
				std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
				double duration = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - mStart).count();
				uint64_t endtime = std::chrono::duration_cast<std::chrono::nanoseconds>(end.time_since_epoch()).count();
				mCurrent_ms += 0.000001 * duration;
				if (mCurrent_ms > mMax_ms)
					mMax_ms = mCurrent_ms;
				mIsStarted = false;
				if (Profiler::doComma) {
					fprintf(Profiler::mFile, ",\n");
				}
				fprintf(Profiler::mFile, "{\"name\": \"%s\", \"ph\": \"E\", \"ts\": %llu, \"pid\": 1, \"tid\": 1}", GetName().c_str(), endtime);
				Profiler::doComma = true;
			}
		}

		/// Reset the timer to zero
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
		// The constructor and destructor are private.  This means you cannot create your own timers, but because it is a friend, the Profiler class can.
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

	// A helper wrapper for Timer that starts and stops the timer automatically when this goes in and out of scope.
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

	/// The Profiler is set up as a singleton class - this means you can only have one of them... you cannot use this with multithreading.
	static Profiler *Get();

	/// The Timers are stored in a map to be looked up by name
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

	/// Reset all Timers.  You would be expected to call this once per frame.
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

// This handy macro lets you create a ScopedTimer by name with a single function-like macro
#define PROFILE_SCOPE(name) Profiler::ScopedTimer name_scope(Profiler::Get()->GetTimer(std::string(#name)))