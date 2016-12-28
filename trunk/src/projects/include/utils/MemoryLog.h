#pragma once
#ifndef MemoryLog_h__
#define MemoryLog_h__

#include "utils/TimedLog.h"

namespace utils
{
	class MemoryLog : public TimedLog
	{
	public:
		MemoryLog(QObject* parent = nullptr);

	protected:
		void save(std::ostream& os) override;
	};
}

#endif // MemoryLog_h__