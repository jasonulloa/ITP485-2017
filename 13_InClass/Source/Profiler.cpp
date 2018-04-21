#include "ITPEnginePCH.h"
#include "Profiler.h"

/*static*/ Profiler *Profiler::Get()
{
	static Profiler s_theProfiler;
	return &s_theProfiler;
}

Profiler::Profiler()
{
}

Profiler::~Profiler()
{
	FILE *pFile = nullptr;
	fopen_s(&pFile, "profile.txt", "wt");
	if (nullptr != pFile)
		fprintf(pFile, "name:, avg (ms), max (ms)\n");
	for (auto& iter : mTimers)
	{
		Timer *pTimer = iter.second;
		if (nullptr != pFile)
			fprintf(pFile, "%s:, %f, %f\n", pTimer->GetName().c_str(), (float)pTimer->GetAvg_ms(), (float)pTimer->GetMax_ms());
		delete pTimer;
	}
	mTimers.clear();
	fclose(pFile);
}