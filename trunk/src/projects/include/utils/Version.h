#pragma once
#ifndef Version_h__
#define Version_h__

#include <string>

namespace utils
{
	class Version
	{
	public:
		static const std::string& getVersionString();
		static const std::string& getBranchName();

		static const std::string& getFullVersion();

	private:
		static const unsigned SVN_REVISION;
		static const unsigned VERSION_MAJOR;
		static const unsigned VERSION_MINOR;
	};
}

#endif // Version_h__