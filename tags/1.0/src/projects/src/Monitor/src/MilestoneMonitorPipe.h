#pragma once
#ifndef MilestoneMonitorPipe_h__
#define MilestoneMonitorPipe_h__

#include "AbstractMonitor.h"

#include <utils/PipeProcess.h>
#include <utils/BlockingQueue.h>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include <opencv2/core.hpp>

namespace utils
{
	namespace camera 
	{
		class AlertSender;
	}
}

/// <summary>
/// Klasa zarz¹dzaj¹ca potokiem w wersji do pracy z oprogramowaniem Milestone. Zapewnia funkcjonalnoæ przetwarzania plików potoku. 
/// </summary>
/// <seealso cref="AbstractMonitor" />
class MilestoneMonitorPipe : public AbstractMonitor
{
public:
	MilestoneMonitorPipe();
	~MilestoneMonitorPipe();

	static utils::IAppModule* create();

protected:
	virtual void reserve() override;
	virtual void process() override;

private:
	struct FileDesc
	{
		enum Type
		{
			ACTION,
			CLUSTER,
			VIDEO,
		};
		Type type;
		int majorId;
		int minorId;
		bool operator<(const FileDesc& rhs) const;
		std::string suffix() const;
	};

	int m_NextIndex;

	
	void findFiles(std::set<FileDesc> &files);
	void processFiles(const std::set<FileDesc>& files, std::vector<boost::filesystem::path> &filesToDelete);

	void deleteFiles(const std::vector<boost::filesystem::path>& filesToDelete);
};

#endif // MilestoneMonitorPipe_h__