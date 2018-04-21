#include "ITPEnginePCH.h"
#include "Profiler.h"

bool Profiler::doComma;
FILE* Profiler::mFile;

/*static*/ Profiler *Profiler::Get()
{
	static Profiler s_theProfiler;
	return &s_theProfiler;
}

Profiler::Profiler()
{
	Profiler::doComma = false;
	Profiler::mFile = nullptr;
	fopen_s(&Profiler::mFile, "profile.txt", "wt");
	if (nullptr != Profiler::mFile)
		fprintf(Profiler::mFile, "[");
}

//----------------------------------------------------------------------------------------
// Upon destruction (when the program ends), the profile data is dumped out to a text
// file.
//----------------------------------------------------------------------------------------
Profiler::~Profiler()
{
	FILE *pFile = nullptr;
	fopen_s(&pFile, "profile_excel.txt", "wt");
	if (nullptr != pFile)
		fprintf(pFile, "name:, avg (ms), max (ms)\n");
	for (auto& iter : mTimers)
	{
		Timer *pTimer = iter.second;
		if (nullptr != pFile) {
			fprintf(pFile, "%s:, %f, %f\n", pTimer->GetName().c_str(), (float)pTimer->GetAvg_ms(), (float)pTimer->GetMax_ms());
		}
		delete pTimer;
	}
	fclose(pFile);
	fprintf(Profiler::mFile, "]");
	mTimers.clear();
	fclose(Profiler::mFile);
}