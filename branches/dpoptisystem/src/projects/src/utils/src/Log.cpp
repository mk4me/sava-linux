#include "Log.h"

#include "Filesystem.h"

#include <boost/filesystem.hpp>

#include <QtCore/QCoreApplication>

namespace utils
{
	Log::Log(bool clear /*= false*/)
	{
		std::string dir = Filesystem::unifyPath(Filesystem::getUserPath() + "debugLogs");

		boost::filesystem::create_directories(dir);

		m_File.open(dir + QCoreApplication::applicationName().toStdString() + ".log", clear ? std::ios::out : std::ios::app);
	}

	void Log::clear()
	{
		Log log(true);
	}

}