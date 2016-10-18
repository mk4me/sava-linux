#include "PathAnalyzer.h"

#include <sequence/Cluster.h>

#include <utils/FileLock.h>
#include <utils/Filesystem.h>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

clustering::PathAnalyzer::PathAnalyzer() : m_MinProcessTime(0)
{

}

clustering::PathAnalyzer::~PathAnalyzer()
{

}

void clustering::PathAnalyzer::setFiles(const std::string& inputSequence, const std::string& inputPaths, const std::string& outputClusterPattern, const std::string& outputClusterExtension)
{
	m_InputSequence = inputSequence;
	m_InputPaths = inputPaths;
	m_OutputClusterPattern = outputClusterPattern;
	m_OutputClusterExtension = outputClusterExtension;
	m_OutputClusterId = 0;
}

std::string clustering::PathAnalyzer::getInFileName() const
{
	return m_InputPaths;
}

std::string clustering::PathAnalyzer::getVideoFileName() const
{
	return m_InputSequence;
}

std::string clustering::PathAnalyzer::getOutFileName(unsigned id) const
{
	return m_OutputClusterPattern + std::to_string(id) + m_OutputClusterExtension;
}

void clustering::PathAnalyzer::saveCluster(sequence::Cluster& cluster)
{
	std::string outFile = getOutFileName(m_OutputClusterId++);
	utils::FileLock fileLock(outFile + ".loc");
	if (!fileLock.lock())
	{
		std::cerr << "PathAnalysis::save() can't lock file " << outFile << ".loc" << std::endl;
	}
	else
	{
		cluster.save(outFile);
	}
}

void clustering::PathAnalyzer::cleanup()
{
	try
	{
		if (utils::Filesystem::exists(getInFileName()))
			boost::filesystem::remove(getInFileName());
	}
	catch (...)
	{
		std::cerr << "clustering::PathAnalyzer::cleanup(): Can't remove file \"" << getInFileName() << "\"" << std::endl;
	}
}

void clustering::PathAnalyzer::waitIdle()
{
	// FIX na zbyt wolne odswiezanie systemu plikow
	if (m_MinProcessTime > 0)
	{
		auto elapsedTime = m_Timer.elapsed().wall / 1000;
		if (elapsedTime < m_MinProcessTime * 1000)
		{
			long long leftTime = m_MinProcessTime * 1000 - elapsedTime;
			boost::this_thread::sleep_for(boost::chrono::microseconds(leftTime));
		}
		m_Timer.start();
	}
}
