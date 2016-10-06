#include "EditorSequencesManager.h"
#include "boost\mpl\assert.hpp"

#include <QtCore\QFileInfo>
#include "include\EditorMessageManager.h"

/*--------------------------------------------------------------------*/
EditorSequencesManager::EditorSequencesManager()
{
	m_Explorer = EditorSequencesExplorerPtr(new EditorSequencesExplorer());
	m_Sequence = EditorSequencePtr(new EditorSequence());
}

/*--------------------------------------------------------------------*/
EditorSequencesManager::~EditorSequencesManager()
{
	
}

/*--------------------------------------------------------------------*/
bool EditorSequencesManager::load(const std::string& i_VideoFilePath)
{
	EditorMessageManager::sendMessage(MESSAGE_NEW_SEQUENCE_LOAD_START);

	bool isLoaded = false;

	//get input file info
	QFileInfo fileInfo(i_VideoFilePath.c_str());
	
	if (fileInfo.exists())
	{
		//get input file directory
		std::string l_WorkingDir = fileInfo.absoluteDir().absolutePath().toStdString();

		//try to save current sequence
		save();

		//check if explorer is loaded and the working dir is equal to input one, if not load again
		if (!m_Explorer->isLoaded() || l_WorkingDir != m_Explorer->getWorkingDir())
		{
			m_Explorer->load(l_WorkingDir);
		}

		isLoaded = loadSequence(i_VideoFilePath);
	}

	EditorMessageManager::sendMessage(MESSAGE_NEW_SEQUENCE_LOAD_FINISH, isLoaded ? 1 : 0);

	return isLoaded;
}

/*--------------------------------------------------------------------*/
void EditorSequencesManager::save() const
{
	if (!m_Explorer->isLoaded())
		return;

	QFileInfo videoInfo = QFileInfo(m_Sequence->getVideoPath().c_str());
	if (!videoInfo.exists())
		return;


	std::string l_VideoFileName = videoInfo.fileName().toStdString();

	//get working dir cluster files by current video 
	std::vector<std::string> l_ClusterFilesToDelete = m_Explorer->getSequenceClusters(l_VideoFileName);
	std::vector<std::string> l_ActionFilesToDelete = m_Explorer->getSequenceActions(l_VideoFileName);

	//save all clusters
	std::map<size_t, ClusterPtr> l_Clusters = m_Sequence->getClusters();
	for (auto it = l_Clusters.begin(); it != l_Clusters.end(); ++it)
	{
		int id = it->first;
		ClusterPtr clusterPtr = it->second;

		std::string clusterFileName = QString("%1.%2.%3").arg(videoInfo.completeBaseName()).arg(id).arg(EditorSequencesExplorer::CLUSTER_EXTENSION).toStdString();
		std::string clusterFullPath = m_Explorer->createFile(clusterFileName);
		clusterPtr->save(clusterFullPath);

		auto foundedIt = std::find(l_ClusterFilesToDelete.begin(), l_ClusterFilesToDelete.end(), clusterFileName);
		if (foundedIt != l_ClusterFilesToDelete.end())
			l_ClusterFilesToDelete.erase(foundedIt);


		ActionPtr actionPtr = m_Sequence->getAction(id);
		if (actionPtr->getActionId() >= 0)
		{
			std::string actionFileName = QString("%1.%2.%3").arg(videoInfo.completeBaseName()).arg(id).arg(EditorSequencesExplorer::ACTION_EXTENSION).toStdString();
			std::string actionFullPath = m_Explorer->createFile(actionFileName);

			actionPtr->save(actionFullPath);
			auto foundedIt = std::find(l_ActionFilesToDelete.begin(), l_ActionFilesToDelete.end(), actionFileName);
			if (foundedIt != l_ActionFilesToDelete.end())
				l_ActionFilesToDelete.erase(foundedIt);
		}
			
	}

	//delete cluster files
	for (std::string clusterFile : l_ClusterFilesToDelete)
		m_Explorer->deleteFile(clusterFile);


	//delete action files
	for (std::string actionFile : l_ActionFilesToDelete)
		m_Explorer->deleteFile(actionFile);


	m_Explorer->reloadSequenceFiles(l_VideoFileName);
}

/*--------------------------------------------------------------------*/
bool EditorSequencesManager::loadNext()
{
	bool isLoaded = false;

	std::string nextVideo = getNextVideoFile();
	if (!nextVideo.empty())
		isLoaded = load(nextVideo);

	return isLoaded;
}

/*--------------------------------------------------------------------*/
bool EditorSequencesManager::loadPrev()
{
	bool isLoaded = false;

	std::string preVideo = getPrevVideoFile();
	if (!preVideo.empty())
		isLoaded = load(preVideo);

	return isLoaded;
}

/*--------------------------------------------------------------------*/
bool EditorSequencesManager::isLoaded() const
{
	return !m_Sequence->getVideoPath().empty();
}

/*--------------------------------------------------------------------*/
bool EditorSequencesManager::loadSequence(const std::string& i_VideoPath)
{
	if (!m_Explorer->isLoaded())
		return false;

	//load video sequence
	m_Sequence->clear();
	m_Sequence->loadVideo(i_VideoPath);

	//load sequence cluster and actions
	std::string videoFileName = QFileInfo(i_VideoPath.c_str()).fileName().toStdString();
	std::vector<std::string> clusters = m_Explorer->getSequenceClusters(videoFileName);
	for (auto it = clusters.begin(); it != clusters.end(); ++it)
	{
		//create cluster
		int l_Id = m_Explorer->convertFileNameToInt(*it);
		assert(l_Id >= 0);

		ClusterPtr l_ClusterPtr = ClusterPtr(new Cluster(m_Explorer->getAbsolutePath(*it)));

		//create action
		std::string l_ActionFile = m_Explorer->getActionByCluster(*it);
		ActionPtr l_ActionPtr = nullptr;
		if (!l_ActionFile.empty())
			l_ActionPtr = ActionPtr(new Action(m_Explorer->getAbsolutePath(l_ActionFile)));

		
		m_Sequence->addCluster(l_Id, l_ClusterPtr, l_ActionPtr);
	}

	return true;
}


/*--------------------------------------------------------------------*/
std::string EditorSequencesManager::getNextVideoFile() const
{
	std::string currVideoFileName = QFileInfo(m_Sequence->getVideoPath().c_str()).fileName().toStdString();
	if (!currVideoFileName.empty())
	{
		std::vector<std::string> allSequences = m_Explorer->getSequences();
		auto foundIt = std::find(allSequences.begin(), allSequences.end(), currVideoFileName);
		if (foundIt != allSequences.end() && (foundIt + 1) != allSequences.end())
			return m_Explorer->getAbsolutePath(*(foundIt + 1));
	}

	return std::string();
}

/*--------------------------------------------------------------------*/
std::string EditorSequencesManager::getPrevVideoFile() const
{
	std::string currVideoFileName = QFileInfo(m_Sequence->getVideoPath().c_str()).fileName().toStdString();
	if (!currVideoFileName.empty())
	{
		std::vector<std::string> allSequences = m_Explorer->getSequences();
		auto foundIt = std::find(allSequences.begin(), allSequences.end(), currVideoFileName);
		if (foundIt != allSequences.end() && foundIt != allSequences.begin())
			return m_Explorer->getAbsolutePath(*(foundIt - 1));
	}

	return std::string();
}
