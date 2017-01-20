#include "Filesystem.h"

#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

#include <QtCore/QCoreApplication>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#define MAX_PATH 500
#endif
#include <iostream>

namespace utils
{
	std::string Filesystem::unifyPath(const std::string& path)
	{
#ifdef _WIN32
		const char fromSeparator = '/';
		const char toSeparator = '\\';
#else
		const char fromSeparator = '\\';
		const char toSeparator = '/';
#endif // _WIN32

		if (path.empty())
			return path;

		std::string p = path;
		std::replace(p.begin(), p.end(), fromSeparator, toSeparator);
		if (p.back() != toSeparator)
			p += toSeparator;

		return p;
	}

	const std::string& Filesystem::getAppPath()
	{
		static std::string appPath = unifyPath(getOsAppPath());
		return appPath;
	}

	const std::string& Filesystem::getDataPath()
	{
		static std::string dataPath = unifyPath(getAppPath() + "../data/");
		return dataPath;
	}

	const std::string& Filesystem::getConfigPath()
	{
		//static std::string configPath = unifyPath(getDataPath() + "config/");
        static std::string configPath = unifyPath(getAppPath() + "../../../input/configin/");
		return configPath;
	}

	const std::string& Filesystem::getUserPath()
	{
		static std::string userPath = unifyPath(getAppPath() + "../user/");
		return userPath;
	}

	int Filesystem::getNumDirs(const std::string& directory)
	{
		if (!utils::Filesystem::exists(directory))
			return 0;

		return std::count_if(
			boost::filesystem::directory_iterator(directory),
			boost::filesystem::directory_iterator(),
			boost::bind(static_cast<bool(*)(const boost::filesystem::path&)>(boost::filesystem::is_directory),
			boost::bind(&boost::filesystem::directory_entry::path, _1)));
	}

	bool Filesystem::exists(const boost::filesystem::path& path)
	{
		try
		{
			return boost::filesystem::exists(path);
		}
		catch (...)
		{
			return false;
		}
	}

	bool Filesystem::exists(const std::string& path)
	{
		return exists(boost::filesystem::path(path));
	}

	void Filesystem::removeContents(const std::string& directory)
	{
		if (!exists(directory))
			return;

		boost::filesystem::directory_iterator endIt;
		for (boost::filesystem::directory_iterator it(directory); it != endIt; ++it)
		{
			try
			{
				boost::filesystem::remove_all(*it);
			}
			catch (const boost::filesystem::filesystem_error&)
			{
				std::cerr << "Filesystem error: Cannot remove " << it->path() << std::endl;
			}
		}
	}

	std::vector<std::string> Filesystem::getFileList(const std::string& directory)
	{
		std::vector<std::string> fileList;

		try
		{
			if (!utils::Filesystem::exists(directory))
				return fileList;

			boost::filesystem::directory_iterator endIt;
			for (boost::filesystem::directory_iterator it(directory); it != endIt; ++it)
			{
				if (!utils::Filesystem::exists(*it) || !boost::filesystem::is_regular_file(*it))
					continue;

				fileList.push_back(it->path().string());
			}
		}
		catch (const boost::filesystem::filesystem_error&)
		{
			return fileList;
		}

		return fileList;
	}

	std::string Filesystem::getOsAppPath()
	{
		char buffer[MAX_PATH];
		#ifdef _WIN32
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		#else
		readlink("/proc/self/exe", buffer, MAX_PATH);
   		#endif // _WIN32
		size_t pos = std::string(buffer).find_last_of("\\/");
		return std::string(buffer).substr(0, pos);
	}
}
