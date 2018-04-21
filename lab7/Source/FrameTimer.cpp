#include "ITPEnginePCH.h"
#include "Profiler.h"

FrameTimer::FrameTimer()
{
#if _WIN32
	QueryPerformanceFrequency(&mFreq);
#else
#endif
}

void FrameTimer::Start()
{
#ifdef _WIN32
	QueryPerformanceFrequency(&mFreq);
	QueryPerformanceCounter(&mStart);
#else
	mStart = std::chrono::high_resolution_clock::now();
#endif
}

float FrameTimer::GetFrameTime(float lockTime /*= -1.0f*/)
{
	PROFILE_SCOPE(FrameTimer_GetFrameTime);
	float time = GetElapsed();

	if (lockTime > 0.0f)
	{
		PROFILE_SCOPE(FrameTimer_GetFrameTimer_LockTime);
		// Lock the frame rate
		while (time < lockTime)
		{
			time = GetElapsed();
		}

		time = lockTime;
	}

	// Restart the timer for next time
	Start();

	return time;
}

float FrameTimer::GetElapsed() const
{
#ifdef _WIN32
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	float elapsed = static_cast<float>(end.QuadPart - mStart.QuadPart) / mFreq.QuadPart;
	return elapsed;
#else
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - mStart).count();
	return duration / 1000000000.0f;
#endif
}
