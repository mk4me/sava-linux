#include "Version.h"
#ifndef _DEBUG
#include "SVNVersion.h"
#endif

namespace utils
{
#ifndef _DEBUG
	const unsigned Version::SVN_REVISION = SVN_REVISION_IMPL;
#else
	const unsigned Version::SVN_REVISION = 0;
#endif

	const unsigned Version::VERSION_MAJOR = 0;
	const unsigned Version::VERSION_MINOR = 3;

	const std::string& Version::getVersionString()
	{
		static std::string str = std::to_string(VERSION_MAJOR) + "." + std::to_string(VERSION_MINOR) + "." + std::to_string(SVN_REVISION);
		return str;
	}

	const std::string& Version::getBranchName()
	{
#ifndef _DEBUG
		static size_t offset = 47;
		static std::string tmpStr = std::string(SVN_URL_IMPL).substr(offset, std::string(SVN_URL_IMPL).find_first_of('/', offset) - offset);
		if (tmpStr == "branches")
		{
			static std::string str = std::string(SVN_URL_IMPL).substr(offset + 9, std::string(SVN_URL_IMPL).find_first_of('/', offset + 9) - offset - 9);
			return str;
		}
		else if(tmpStr == "tags")
		{
			static std::string str = std::string(SVN_URL_IMPL).substr(offset + 5, std::string(SVN_URL_IMPL).find_first_of('/', offset + 5) - offset - 5);
			return str;
		}
		else
		{
			static std::string str = tmpStr;
			return str;
		}
#else
		static std::string str = "DEBUG";
		return str;
#endif
	}

	const std::string& Version::getFullVersion()
	{
		static std::string str = getVersionString() + (getBranchName() == "trunk" ? std::string() : " " + getBranchName());
		return str;
	}

}