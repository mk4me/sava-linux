#include "MemoryLog.h"

#ifdef WIN32
#include "windows.h"
#include "psapi.h"
#endif

namespace utils
{
	MemoryLog::MemoryLog(QObject* parent /*= nullptr*/)
		: TimedLog(parent)
	{
		startTimer("memory", 60.0f);
	}

	void MemoryLog::save(std::ostream& os)
	{
		unsigned long long usedMem = 0;
#ifdef WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		usedMem = pmc.PrivateUsage;
#endif
		os << usedMem / 1024 << '\n';
	}

}