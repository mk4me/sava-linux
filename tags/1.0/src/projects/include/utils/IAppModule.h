#ifndef IProcessor_h__
#define IProcessor_h__

#include "ProgramOptions.h"

#include <string>
#include <utility>
#include <vector>

namespace utils
{
	/// <summary>
	/// interfejs modu³u rejestrowanego w klasie Application.
	/// </summary>
	class IAppModule
	{
	public:
		typedef IAppModule*(*CreateFunction)(void);

		virtual ~IAppModule() { }

		/// <summary>
		/// Starts this instance.
		/// </summary>
		/// <returns></returns>
		virtual bool start() = 0;
		/// <summary>
		/// Stops this instance.
		/// </summary>
		virtual void stop() = 0;

		/// <summary>
		/// Loads the parameters.
		/// </summary>
		/// <param name="options">The options.</param>
		/// <returns></returns>
		virtual bool loadParameters(const ProgramOptions& options) = 0;
	};
}
#endif // IProcessor_h__
