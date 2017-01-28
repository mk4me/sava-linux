#include "EditorSequence.h"
#include "EditorSequencesManager.h"


EditorSequence::EditorSequence()
	:m_WasModified(false)
{
}

/*--------------------------------------------------------------------*/
void EditorSequence::clear()
{
	m_Clusters.clear();
	m_ClustersFiles.clear();
}


/*--------------------------------------------------------------------*/
void EditorSequence::addCluster(const EditorClusterPtr& i_ClusterPtr)
{
	inner_addCluster(i_ClusterPtr);
	modified();
}

void EditorSequence::inner_addCluster(const EditorClusterPtr& i_ClusterPtr)
{
	static int s_InnserId = 0;

	assert(i_ClusterPtr);
	i_ClusterPtr->setUniqueId(s_InnserId);
	m_Clusters.insert(i_ClusterPtr);

	s_InnserId++;
}


/*--------------------------------------------------------------------*/
size_t EditorSequence::createCluster()
{
	//find next cluster id
	int clusterId = EditorSequencesManager::createUniqueClusterId();

	//finaly we add cluster 
	EditorClusterPtr newCluster = std::make_shared<EditorCluster>(clusterId);
	addCluster(newCluster);

	modified();

	return newCluster->getUniqueId();
}


/*--------------------------------------------------------------------*/
EditorClusterPtr EditorSequence::getCluster(int i_Id)
{
	auto it = std::find_if(m_Clusters.begin(), m_Clusters.end(), [&](const EditorClusterPtr& clu) { return clu->getUniqueId() == (int)i_Id; });// m_Clusters.find(i_Id);
	return (it != m_Clusters.end()) ? *it : nullptr;
}


/*--------------------------------------------------------------------*/
bool EditorSequence::removeCluster(int i_Id)
{
	//remove cluster
	auto it = std::find_if(m_Clusters.begin(), m_Clusters.end(), [&](const EditorClusterPtr& clu) { return clu->getUniqueId() == (int)i_Id; });
	if (it != m_Clusters.end() && (*it)->isEditable())
	{
		m_Clusters.erase(it);
		modified();
		return true;
	}

	return false;
}


/*--------------------------------------------------------------------*/
QRect EditorSequence::getClusterFrame(int m_Id, size_t i_FrameNr)
{
	EditorClusterPtr editorCluster = getCluster(m_Id);
	if (editorCluster)
	{
		ClusterPtr cluster = editorCluster->getSource();

		cv::Rect rect;

		if (i_FrameNr < cluster->getStartFrame())
			rect = cluster->getFrameAt(cluster->getStartFrame());
		else if (i_FrameNr > cluster->getEndFrame())
			rect = cluster->getFrameAt(cluster->getEndFrame());
		else
			rect = cluster->getFrameAt(i_FrameNr);

		return QRect(rect.x, rect.y, rect.width, rect.height);
	}

	return QRect();
}


/*--------------------------------------------------------------------*/
void EditorSequence::addClusterFrame(int m_Id, const QRect& i_Rect, size_t i_FrameNr)
{
	EditorClusterPtr editorCluster = getCluster(m_Id);
	if (editorCluster)
	{
		ClusterPtr cluster = editorCluster->getSource();

		Cluster::FramesPositionsMap points = cluster->getFramesPositions();
		points[i_FrameNr] = cv::Point(i_Rect.x(), i_Rect.y());
		cluster->setFramesPositions(points);
		cluster->setFrameSize(cv::Size(i_Rect.width(), i_Rect.height()));

		editorCluster->update();

		modified();
	}
}

/*--------------------------------------------------------------------*/
std::vector<std::pair<size_t, cv::Rect>> EditorSequence::getClusterFrames(size_t i_FrameNr)
{
	std::vector< std::pair<size_t, cv::Rect>> retVec;
	for (auto it = m_Clusters.begin(); it != m_Clusters.end(); ++it) 
	{
		EditorClusterPtr editorCluster = (*it);
		ClusterPtr cluster = (*it)->getSource();

		if (i_FrameNr >= cluster->getStartFrame() && i_FrameNr <= cluster->getEndFrame())
			retVec.push_back(std::make_pair(editorCluster->getUniqueId(), cluster->getFrameAt(i_FrameNr)));
	}

	return retVec;
}


/*--------------------------------------------------------------------*/
bool EditorSequence::setClusterStartFrame(size_t i_Id, size_t i_StartFrameNr)
{
	EditorClusterPtr editorCluster = getCluster(i_Id);
	if (!editorCluster->isEditable())
		return false;

	ClusterPtr cluster = editorCluster->getSource();

	size_t clusterBeginTime = cluster->getStartFrame();
	size_t clusterEndTime = cluster->getEndFrame();

	if (i_StartFrameNr == clusterBeginTime || i_StartFrameNr >= clusterEndTime)
		return false;

	Cluster::FramesPositionsMap points = cluster->getFramesPositions();

	if (i_StartFrameNr < clusterBeginTime)
	{
		QRect rect = getClusterFrame(i_Id, clusterBeginTime);

		//move current start point to set one
		auto removeIt = points.find(clusterBeginTime);
		if (removeIt != points.end())
			points.erase(removeIt);

		points[i_StartFrameNr] = cv::Point(rect.x(), rect.y());
	}
	else if (i_StartFrameNr > clusterEndTime)
	{
		QRect rect = getClusterFrame(i_Id, clusterEndTime);
		points.clear();
		points[i_StartFrameNr] = cv::Point(rect.x(), rect.y());
	}
	else
	{
		QRect rect = getClusterFrame(i_Id, i_StartFrameNr);
		points[i_StartFrameNr] = cv::Point(rect.x(), rect.y());

		auto it = points.begin();
		for (auto it = points.begin(); it != points.end();)
		{
			if (it->first < i_StartFrameNr)
				points.erase(it++);
			else
				++it;
		}
	}

	cluster->setFramesPositions(points);
	editorCluster->update();

	modified();

	return true;
}


/*--------------------------------------------------------------------*/
bool EditorSequence::setClusterEndFrame(size_t i_Id, size_t i_EndFrameNr)
{
	EditorClusterPtr editorCluster = getCluster(i_Id);
	if (!editorCluster->isEditable())
		return false;

	ClusterPtr cluster = editorCluster->getSource();

	size_t clusterBeginTime = cluster->getStartFrame();
	size_t clusterEndTime = cluster->getEndFrame();

	if (i_EndFrameNr == clusterEndTime || i_EndFrameNr <= clusterBeginTime)
		return false;

	Cluster::FramesPositionsMap points = cluster->getFramesPositions();

	if (i_EndFrameNr > clusterEndTime)
	{
		QRect rect = getClusterFrame(i_Id, clusterEndTime);

		//move current end point to set one
		auto removeIt = points.find(clusterEndTime);
		if (removeIt != points.end())
			points.erase(removeIt);

		points[i_EndFrameNr] = cv::Point(rect.x(), rect.y());
	}
	else if (i_EndFrameNr < clusterBeginTime)
	{
		QRect rect = getClusterFrame(i_Id, clusterBeginTime);
		points.clear();
		points[i_EndFrameNr] = cv::Point(rect.x(), rect.y());
	}
	else
	{
		QRect rect = getClusterFrame(i_Id, i_EndFrameNr);
		points[i_EndFrameNr] = cv::Point(rect.x(), rect.y());

		auto it = points.begin();
		for (auto it = points.begin(); it != points.end();)
		{
			if (it->first > i_EndFrameNr)
				points.erase(it++);
			else
				++it;
		}
	}

	cluster->setFramesPositions(points);
	editorCluster->update();

	modified();

	return true;
}

std::vector<std::string> EditorSequence::getClusterFiles(size_t i_Id)
{
	auto it = m_ClustersFiles.find(i_Id);
	if (it != m_ClustersFiles.end())
		return it->second;

	return std::vector<std::string>();
}

std::vector<EditorClusterPtr> EditorSequence::getClustersByClusterId(int i_ClusterId) const
{
	std::vector<EditorClusterPtr> clusters;
	for (auto clu : m_Clusters)
		if (clu->getSource()->getClusterId() == i_ClusterId)
			clusters.push_back(clu);

	return clusters;
}

void EditorSequence::addClusterFile(size_t clusterId, std::string fileName)
{
	auto filesIt = m_ClustersFiles.find(clusterId);
	if (filesIt == m_ClustersFiles.end())
		m_ClustersFiles.insert(std::make_pair(clusterId, std::vector<std::string>()));

	m_ClustersFiles[clusterId].push_back(fileName);
}
