#ifndef IProcessor_h__
#define IProcessor_h__

#include "ProgramOptions.h"

#include <string>
#include <utility>
#include <vector>

namespace utils
{
	class IAppModule
	{
	public:
		typedef IAppModule*(*CreateFunction)(void);

		virtual ~IAppModule() { }

		virtual bool start() = 0;
		virtual void stop() = 0;

		virtual bool loadParameters(const ProgramOptions& options) = 0;
	};
}
#endif // IProcessor_h__
