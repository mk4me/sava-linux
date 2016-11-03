#pragma once
#ifndef MonitorPipe_h__
#define MonitorPipe_h__

#include "AbstractMonitor.h"

#include <sequence/MetaVideo.h>

//#include <utils\PipeProcess.h>
#include <utils/Singleton.h>
#include <utils/BlockingQueue.h>

#include <boost/thread.hpp>

#include <atomic>

class MonitorWindow;

class MonitorPipe : public AbstractMonitor, public Singleton<MonitorPipe>
{
public:
	class VisualizationCreator
	{
	public:
		virtual ~VisualizationCreator() { }
		virtual void createVisualization() = 0;
	};

	~MonitorPipe();

	static utils::IAppModule* create();
	static void registerParameters(ProgramOptions& programOptions);

	virtual bool loadParameters(const ProgramOptions& options) override;

	virtual bool start() override;
	virtual void stop() override;

	size_t getInputQueueSize() const;
	std::shared_ptr<sequence::MetaVideo> popInputVideo();
	void pushDeleteVideo(const std::shared_ptr<sequence::MetaVideo>& video);
	
	static void setVisualizationCreator(VisualizationCreator* creator) { ms_VisualizationCreator = creator; }

protected:
	virtual void reserve() override;
	virtual void process() override;

private:
	MonitorPipe();

	static VisualizationCreator* ms_VisualizationCreator;

	utils::BlockingQueue<std::shared_ptr<sequence::MetaVideo>> m_InputQueue;
	utils::BlockingQueue<std::shared_ptr<sequence::MetaVideo>> m_DeleteQueue;

	//std::map<std::shared_ptr<sequence::MetaVideo>, std::string> m_VideoToFileName;

	int m_NextIndex;

	boost::thread m_FileRemoveThread;
	std::atomic<int> m_LastToDeleteIndex;

	std::string getFileName(int index) const;

	void fileRemoveThread();
};

#endif // MonitorPipe_h__