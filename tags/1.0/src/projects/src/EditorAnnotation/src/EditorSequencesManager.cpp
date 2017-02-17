
#include "EditorSequencesManager.h"
#include "boost/mpl/assert.hpp"

#include <QtCore/QFileInfo>

#include "EditorMessageManager.h"
#include "EditorTimeline.h"


int EditorSequencesManager::m_UniqueClusterId = 0;

/*--------------------------------------------------------------------*/
EditorSequencesManager::EditorSequencesManager()
	: m_LoadFrameNr(0)
	, m_WasPlaying(false)
	, m_IsLoading(false)
{
	m_Explorer = std::make_shared<EditorSequencesExplorer>();
	m_Sequence = std::make_shared<EditorMultiSequence>();

	connect(&m_LoadThread, SIGNAL(started()), this, SLOT(loadingStarted_internal()));
	connect(&m_LoadThread, SIGNAL(finished()), this, SLOT(loadingFinished_internal()));
}

/*--------------------------------------------------------------------*/
void EditorSequencesManager::load(const std::vector<std::string>& i_VideosPaths)
{
	if (m_IsLoading)
		return;

	m_WasPlaying = EditorTimeline::getInstance().isPlaying();
	EditorTimeline::getInstance().stop();

	//load version with thread;
	m_LoadThread.setFilesToLoad(i_VideosPaths);
	m_LoadThread.start();

	//load version without thread;
	//emit loadingStarted();
	//loadInternal(i_VideosPaths);
	//emit loadingFinished();
}

/*--------------------------------------------------------------------*/
bool EditorSequencesManager::loadInternal(const std::vector<std::string>& i_VideosPaths)
{
	if (i_VideosPaths.empty())
		return false;

	if (!i_VideosPaths.empty())
	{
		//try to load sequence folder
		if (tryLoadExplorer(*i_VideosPaths.begin()))
		{
			m_VideosPaths = i_VideosPaths;

			//try to save current state
			m_Sequence->save();

			//set new video paths
			m_Sequence->setVideosPaths(i_VideosPaths);

			//try load sequence
			return m_Sequence->load();
		}
	}

	return false;
}


/*--------------------------------------------------------------------*/
void EditorSequencesManager::save() const {
	 m_Sequence->save();
}

/*--------------------------------------------------------------------*/
bool EditorSequencesManager::isLoaded() const {
	return !m_IsLoading && m_Explorer->isLoaded();
}

/*--------------------------------------------------------------------*/
int EditorSequencesManager::createUniqueClusterId() {
	return m_UniqueClusterId++;
}


/*--------------------------------------------------------------------*/
bool EditorSequencesManager::loadNext(int frameNr /*= 0*/)
{
	if (m_VideosPaths.empty())
		return false;

	std::string prevVideo = m_Explorer->getRelativePath(m_VideosPaths.back());
	std::vector<std::string> nextVideos;

	//for (int i = 0; i < m_VideosPaths.size(); i++)
	for (int i = 0; i < 1; i++)
	{
		std::string nextVideo = m_Explorer->getNextSequence(prevVideo);
		if (nextVideo.empty())
			break;

		nextVideos.push_back(m_Explorer->getAbsolutePath(nextVideo));
		prevVideo = nextVideo;
	}

	m_LoadFrameNr = frameNr;
	load(nextVideos);
	return true;
}

/*--------------------------------------------------------------------*/
bool EditorSequencesManager::loadPrev(int frameNr /*= 0*/)
{
	if (m_VideosPaths.empty())
		return false;

	std::string nextVideo = m_Explorer->getRelativePath(m_VideosPaths.front());
	std::vector<std::string> prevVideos;
	for (int i = 0; i < 1; i++)
	{
		std::string prevVideo = m_Explorer->getPrevSequence(nextVideo);
		if (prevVideo.empty())
			break;

		prevVideos.push_back(m_Explorer->getAbsolutePath(prevVideo));
		nextVideo = prevVideo;
	}

	std::reverse(prevVideos.begin(), prevVideos.end());

	m_LoadFrameNr = frameNr;
	load(prevVideos);
	return true;
}


/*--------------------------------------------------------------------*/
void EditorSequencesManager::loadingFinished_internal() {

	//load success ?
	bool success = m_LoadThread.isLoadSuccess();

	//first call signal to listeners
	emit loadingFinished(success);

	//set frame nr in current loaded sequence
	if (success)
	{
		EditorTimeline* time = EditorTimeline::getPointer();

		if (m_LoadFrameNr < 0 || m_LoadFrameNr >= time->getFrameCount()) 
			time->setCurrentFrame(time->getFrameCount() - 1); //set last frame
		else
			time->setCurrentFrame(m_LoadFrameNr);

		if (m_WasPlaying)
			EditorTimeline::getInstance().play();
	}

	//reset frame nr
	m_LoadFrameNr = 0;
	m_WasPlaying = false;
	m_IsLoading = false;
}

void EditorSequencesManager::loadingStarted_internal()
{
	m_IsLoading = true;
	emit loadingStarted();
}

/*--------------------------------------------------------------------*/
bool EditorSequencesManager::tryLoadExplorer(const std::string& i_FilePathFromExplorerDir)
{
	std::string workingDir = QFileInfo(i_FilePathFromExplorerDir.c_str()).absoluteDir().absolutePath().toStdString();
	//load sequences explorer if not yet loaded
	if (!m_Explorer->isLoaded() || workingDir != m_Explorer->getWorkingDir())
	{
		bool success = m_Explorer->load(workingDir);
		if (success) onExplorerLoaded();

		return success;
	}
		
	return true;
}

void EditorSequencesManager::onExplorerLoaded()
{
	std::vector<std::string> vidoeFiles = m_Explorer->getSequences();

	//find next unique cluster id
	for (std::string& seqName : vidoeFiles)
		for (std::string& cluName : m_Explorer->getSequenceClusters(seqName))
		{
			Cluster cluster(m_Explorer->getAbsolutePath(cluName));

			if (cluster.getClusterId() >= m_UniqueClusterId)
				m_UniqueClusterId = cluster.getClusterId() + 1;
		}
	
#if VIDEOS_TIMESTAMPS_ENABLED

	//get vidoes timestamps:
	// good_timestamps - take all times for each video (very slow during videos loading)
	// fake_timestamps - take few videos and calculate rest of it (fast, but imprecise)

#pragma region good_timestamps
	//for (std::string& seqName : m_Explorer->getSequences())
	//{
	//	std::shared_ptr<sequence::IVideo> v = sequence::IVideo::create(m_Explorer->getAbsolutePath(seqName));
	//	size_t num = v->getNumFrames();
	//	if (num > 0)
	//		m_VideosTimestamps.insert(std::make_pair(seqName, std::make_pair(v->getFrameTime(0), v->getFrameTime(num - 1))));
	//}
#pragma endregion good_timestamps

#pragma region fake_timestams 
	std::shared_ptr<sequence::IVideo> firstVideo = sequence::IVideo::create(m_Explorer->getAbsolutePath(vidoeFiles.front()));

	size_t numFrames = firstVideo->getNumFrames();
	sequence::IVideo::Timestamp beginTime = firstVideo->getFrameTime(0);
	sequence::IVideo::Timestamp endTime = firstVideo->getFrameTime(numFrames - 1);
	boost::posix_time::time_duration durationTime = endTime - beginTime;

	for (std::string& seqName : vidoeFiles)
	{
		m_VideosTimestamps.insert(std::make_pair(seqName, std::make_pair(beginTime, endTime)));
		beginTime += durationTime;
		endTime += durationTime;
	}
#pragma endregion fake_timestamps
	
#endif

}

/*--------------------------------------------------------------------*/
EditorSequencePtr EditorSequencesManager::createSequenceByType(EEditorSequenceType _type) const
{
	switch (_type)
	{
		case SINGLE: return EditorSequencePtr(new EditorSingleSequence());
		case MULTI: return EditorSequencePtr(new EditorMultiSequence());
	}

	return nullptr;
}

/*--------------------------------------------------------------------*/
template <typename T>
std::shared_ptr<T> EditorSequencesManager::castSequence(const EditorSequencePtr& _castSeq) const
{
	return std::dynamic_pointer_cast<T>(_castSeq);
}


#if VIDEOS_TIMESTAMPS_ENABLED
/*--------------------------------------------------------------------*/
sequence::IVideo::Timestamp EditorSequencesManager::getVideoFirstTime(const std::string& seqVideoFile) const
{
	auto it = m_VideosTimestamps.find(seqVideoFile);
	if (it != m_VideosTimestamps.end())
		return it->second.first;

	return sequence::IVideo::Timestamp();
}

/*--------------------------------------------------------------------*/
sequence::IVideo::Timestamp EditorSequencesManager::getVideoLastTime(const std::string& seqVideoFile) const
{
	auto it = m_VideosTimestamps.find(seqVideoFile);
	if (it != m_VideosTimestamps.end())
		return it->second.second;

	return sequence::IVideo::Timestamp();
}

#endif




/*------------------------------- LOADING THRED CLASS-------------------------------------*/
void EditorSequencesManager::LoadThread::run() {
	m_LoadSuccess = EditorSequencesManager::getPointer()->loadInternal(m_Files);
}
