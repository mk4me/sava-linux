#include "EditorSequence.h"
#include "EditorSequencesExplorer.h"

/*--------------------------------------------------------------------*/
EditorSequence::EditorSequence():
m_VidePath(""),
m_Video(nullptr)
{

}

/*--------------------------------------------------------------------*/
void EditorSequence::loadVideo(const std::string& i_VideoPath)
{
	clear();

	m_VidePath = i_VideoPath;
	m_Video = IVideo::create(i_VideoPath);
}

/*--------------------------------------------------------------------*/
size_t EditorSequence::createCluster()
{
	//first find free key
	size_t foundKey = 0;
	for (auto it = m_ClusterMap.begin(); it != m_ClusterMap.end(); ++it) {
		if (it->first != foundKey)
			break;
		foundKey++;
	}

	//find next cluster id
	size_t clusterId = 0;
	auto maxIt = std::max_element(m_ClusterMap.begin(), m_ClusterMap.end(), [](std::pair<size_t, ClusterPtr> a, std::pair<size_t, ClusterPtr> b) {
		return a.second->getClusterId() < b.second->getClusterId();
	});

	if (maxIt != m_ClusterMap.end())
		clusterId = maxIt->second->getClusterId() + 1;

	//finaly we add cluster
	addCluster(foundKey, ClusterPtr(new Cluster(clusterId)));

	return foundKey;
}

/*--------------------------------------------------------------------*/
void EditorSequence::addCluster (size_t i_Id, ClusterPtr i_ClusterPtr, ActionPtr i_ActionPtr /* = nullptr */)
{
	assert(i_Id >= 0);
	assert(i_ClusterPtr);

	if (i_ActionPtr == nullptr)
		i_ActionPtr = ActionPtr(new Action(i_ClusterPtr->getClusterId()));

	m_ClusterMap[i_Id] = i_ClusterPtr;
	m_ActionMap[i_Id] = i_ActionPtr;
}

/*--------------------------------------------------------------------*/
void EditorSequence::removeCluster(size_t i_Id)
{
	//remove cluster
	auto foundClusterIt = m_ClusterMap.find(i_Id);
	if (foundClusterIt != m_ClusterMap.end())
		m_ClusterMap.erase(foundClusterIt);

	//remove also action
	auto foundActionIt = m_ActionMap.find(i_Id);
	if (foundActionIt != m_ActionMap.end())
		m_ActionMap.erase(foundActionIt);

}

/*--------------------------------------------------------------------*/
void EditorSequence::setClusterStartFrame(size_t i_Id, size_t i_StartFrameNr)
{
	ClusterPtr cluster = getCluster(i_Id);
	size_t clusterBeginTime = cluster->getStartFrame();
	size_t clusterEndTime = cluster->getEndFrame();

	if (i_StartFrameNr == clusterBeginTime)
		return;

	Cluster::FramesPositionsMap points = cluster->getFramesPositions();

	if (i_StartFrameNr < clusterBeginTime)
	{
		QRect rect = getClusterFrame(i_Id, clusterBeginTime);
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
}

/*--------------------------------------------------------------------*/
void EditorSequence::setClusterEndFrame(size_t i_Id, size_t i_EndFrameNr)
{
	ClusterPtr cluster = getCluster(i_Id);
	size_t clusterBeginTime = cluster->getStartFrame();
	size_t clusterEndTime = cluster->getEndFrame();

	if (i_EndFrameNr == clusterBeginTime)
		return;

	Cluster::FramesPositionsMap points = cluster->getFramesPositions();

	if (i_EndFrameNr > clusterEndTime)
	{
		QRect rect = getClusterFrame(i_Id, clusterEndTime);
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
}

/*--------------------------------------------------------------------*/
QRect EditorSequence::getClusterFrame(size_t i_ClusterId, size_t i_FrameNr)
{
	ClusterPtr cluster = getCluster(i_ClusterId);
	if (cluster)
	{
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
void EditorSequence::addClusterFrame(size_t i_ClusterId, QRect& i_Rect, size_t i_FrameNr)
{
	ClusterPtr cluster = getCluster(i_ClusterId);
	if (cluster)
	{
		Cluster::FramesPositionsMap points = cluster->getFramesPositions();
		points[i_FrameNr] = cv::Point(i_Rect.x(), i_Rect.y());
		cluster->setFramesPositions(points);
		cluster->setFrameSize(cv::Size(i_Rect.width(), i_Rect.height()));
	}
}

/*--------------------------------------------------------------------*/
std::vector<std::pair<size_t, cv::Rect>> EditorSequence::getClusterFrames(size_t i_FrameNr)
{
	std::vector< std::pair<size_t, cv::Rect>> retVec;
	for (auto it = m_ClusterMap.begin(); it != m_ClusterMap.end(); ++it)
		if (i_FrameNr >= it->second->getStartFrame() && i_FrameNr <= it->second->getEndFrame())
			retVec.push_back(std::make_pair(it->first, it->second->getFrameAt(i_FrameNr)));
		
	return retVec;
}

/*--------------------------------------------------------------------*/
std::shared_ptr<Cluster> EditorSequence::getCluster(size_t i_Id)
{
	auto it = m_ClusterMap.find(i_Id);
	return (it != m_ClusterMap.end()) ? it->second : nullptr;
}

/*--------------------------------------------------------------------*/
std::shared_ptr<Action> EditorSequence::getAction(size_t i_Id)
{
	auto it = m_ActionMap.find(i_Id);
	return (it != m_ActionMap.end()) ? it->second : nullptr;
}

/*--------------------------------------------------------------------*/
void EditorSequence::setAction(size_t i_Id, std::shared_ptr<Action> i_ActionPtr)
{
	auto it = m_ClusterMap.find(i_Id);
	if (it != m_ClusterMap.end())
		m_ActionMap[i_Id] = i_ActionPtr;
}

/*--------------------------------------------------------------------*/
void EditorSequence::clear()
{
	for (auto it = m_ClusterMap.begin(); it != m_ClusterMap.end(); ++it)
		it->second.reset();

	for (auto it = m_ActionMap.begin(); it != m_ActionMap.end(); ++it)
		it->second.reset();

	m_VidePath.clear();
	m_ClusterMap.clear();
	m_ActionMap.clear();
	m_Video.reset();
}
