#ifdef WIN32
#include <windows.h>
#include <Psapi.h>
#undef min
#undef max

namespace utils{ 

class Memory
{
public:
	static Memory& instance() 
	{
		static Memory mem;
		return mem;
	}
	
	::size_t getProcessVirtual()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		return pmc.PagefileUsage;
	}

	::size_t getProcessPhysical()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		return pmc.WorkingSetSize;
	}

protected:
	
	MEMORYSTATUSEX memInfo;
	
	Memory() 
	{
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
	}

};


}

#endif