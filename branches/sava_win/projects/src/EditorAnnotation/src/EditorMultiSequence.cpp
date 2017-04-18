#include "EditorMultiSequence.h"
#include "EditorSequencesExplorer.h"
#include "EditorSequencesManager.h"

#include <QtCore/QString>
#include <QtCore/QFile>
#include <assert.h>
#include "EditorTimeline.h"

EditorMultiSequence::EditorMultiSequence()
	: EditorSequence()
{
}


bool EditorMultiSequence::load()
{
	assert(!m_VideoPaths.empty());

	std::map<size_t, std::vector<EditorClusterPtr>> loadedClusters;
	std::map<size_t, std::vector<size_t>> vidoesIds;

	EditorSequencesExplorerPtr explorer = EditorSequencesManager::getInstance().getExplorer();

	clear();
	m_VideosFramesSizes.resize(m_VideoPaths.size(), 0);

	//iterates through all video files and load all clusters and actions
	for (int i = 0; i < m_VideoPaths.size(); i++)
	{
		//load video
		auto video = sequence::IVideo::create(m_VideoPaths.at(i));
		if (!video)
			return false;

		//add to videos
		m_Videos.push_back(video);

		//create video frame sizes 
		m_VideosFramesSizes[i] = (i == 0) ? (video->getNumFrames()) : (m_VideosFramesSizes[i - 1] + video->getNumFrames());

		//create video times vector
		//for (size_t i = 0; i < video->getNumFrames(); i++)
		//	m_FramesTimes.push_back(video->getFrameTime(i));

		std::string videoName = explorer->getRelativePath(m_VideoPaths.at(i));
		std::vector<std::string> clusters = explorer->getSequenceClusters(videoName);
		for (std::string clusterName : clusters)
		{
			//create cluster
			EditorClusterPtr cluster = std::make_shared<EditorCluster>(explorer->getAbsolutePath(clusterName));
			int clusterId = cluster->getSource()->getClusterId();
			if (clusterId < 0)
				continue;

			//create action
			std::string actionName = explorer->getClusterAction(clusterName);
			if (!actionName.empty())
			{
				EditorActionPtr action = std::make_shared<EditorAction>(explorer->getAbsolutePath(actionName));
				*cluster->getAction() = *action;
				cluster->update();
			}

			loadedClusters[clusterId].push_back(cluster);
			vidoesIds[clusterId].push_back(i);
		}
	}

	
	for (auto& clusterIt : loadedClusters)
		for (auto& cluster : clusterIt.second)
			fixCluster(cluster);
	

	//create cluster from multi clusters with the same clusterId
	for (auto& clusterIt : loadedClusters)
	{
		//EditorClusterPtr cluster = merge(clusterIt.second, vidoesIds[clusterIt.first]);

		auto clusters = clusterIt.second;
		auto videoIds = vidoesIds[clusterIt.first];

		bool forceMerge = false;
		std::vector<EditorClusterPtr> clustersToSplit;
		std::vector<size_t> clustersVideoIds;

		//split clusters, when they have diffrent frame size
		for (int i = 0, j = 0; i < clusters.size(); i++)
		{
			if (clustersToSplit.empty())
			{
				clustersToSplit.push_back(clusters.at(i));
				clustersVideoIds.push_back(videoIds.at(i));
				j++;
			}
			else
			{
				cv::Size prevSize = clustersToSplit.at(j - 1)->getSource()->getFrameSize();
				cv::Size currSize = clusters.at(i)->getSource()->getFrameSize();
				if (prevSize == currSize)
				{
					clustersToSplit.push_back(clusters.at(i));
					clustersVideoIds.push_back(videoIds.at(i));
					j++;
				}
				else
				{
					addCluster(merge(clustersToSplit, clustersVideoIds));

					clustersToSplit.clear();
					clustersVideoIds.clear();
					clustersToSplit.push_back(clusters.at(i));
					clustersVideoIds.push_back(videoIds.at(i));
					j = 1;
				}
			}
		}

		if (!clustersToSplit.empty())
			addCluster(merge(clustersToSplit, clustersVideoIds));
	}


	////----- add extra cluster info -----

	////get prev video file
	std::string prevVideoFile = explorer->getPrevSequence(explorer->getRelativePath(m_VideoPaths.front()));
	if (!prevVideoFile.empty())
	{
		auto clustersFiles = explorer->getSequenceClusters(prevVideoFile);
		for (auto& cluFile : clustersFiles)
		{
			Cluster clu = Cluster(explorer->getAbsolutePath(cluFile));
			for (auto it: m_Clusters)
				if (it->getSource()->getClusterId() == clu.getClusterId())
					it->setHasPrevContinuation(true);
		}
	}

	//get next vidoe file
	std::string nextVideoFile = explorer->getNextSequence(explorer->getRelativePath(m_VideoPaths.back()));
	if (!nextVideoFile.empty())
	{
		auto clustersFiles = explorer->getSequenceClusters(nextVideoFile);
		for (auto& cluFile : clustersFiles)
		{
			Cluster clu = Cluster(explorer->getAbsolutePath(cluFile));
			for (auto it : m_Clusters)
				if (it->getSource()->getClusterId() == clu.getClusterId())
					it->setHasNextContinuation(true);
		}
	}
	return true;
}

bool EditorMultiSequence::save()
{
	EditorSequencesExplorerPtr explorer = EditorSequencesManager::getInstance().getExplorer();
	if (!explorer->isLoaded())
		return false;

	if (!wasModified())
		return true;

	std::vector<QFileInfo> videoFileInfoVec;
	std::vector<size_t> fileClusterIds;
	std::map<std::string, std::vector<std::string>> clustersVec;
	std::map<std::string, std::vector<std::string>> actionsVec;

	for (std::string& s : m_VideoPaths) 
	{
		QFileInfo info(s.c_str());
		std::string fileName = info.fileName().toStdString();

		videoFileInfoVec.push_back(info);
		clustersVec.insert(std::make_pair(fileName, std::vector<std::string>()));
		actionsVec.insert(std::make_pair(fileName, std::vector<std::string>()));
		fileClusterIds.push_back(0);
	}

	//save clusters and actions in sequence working dir
	for (auto& clusterIt : getClusters())
	{
		int clusterId = clusterIt->getUniqueId();

		std::vector<size_t> vidoeIds;
		std::vector<EditorClusterPtr> clusters = split(clusterIt, vidoeIds);

		for (int i = 0; i < clusters.size(); i++)
		{
			EditorClusterPtr cluster = clusters[i];
			int videoId = vidoeIds[i];

			QString prefixFileName = QString("%1.%2").arg(videoFileInfoVec[videoId].completeBaseName()).arg(fileClusterIds[videoId]);
			std::string clusterFileName;
			std::string actionFileName;

			clusterFileName = QString("%1.%2").arg(prefixFileName).arg(EditorSequencesExplorer::CLUSTER_EXTENSION).toStdString();
			cluster->getSource()->save(explorer->getAbsolutePath(clusterFileName));

			EditorActionPtr action = clusters[i]->getAction();
			if (action->getSource()->getActionId() >= 0)
			{
				actionFileName = QString("%1.%2").arg(prefixFileName).arg(EditorSequencesExplorer::ACTION_EXTENSION).toStdString();
				action->getSource()->save(explorer->getAbsolutePath(actionFileName));
			}

			//save to clusters and actions vectors
			std::string videoFileName = videoFileInfoVec[videoId].fileName().toStdString();
			clustersVec[videoFileName].push_back(clusterFileName);
			actionsVec[videoFileName].push_back(actionFileName);

			fileClusterIds[videoId]++;


			//push file name to cluster file map
			addClusterFile(clusterId, clusterFileName);
		}
	}


	////synchronize explorere with sequence content
	for (auto& elemIt : clustersVec)
		explorer->refreshClustersAndActions(elemIt.first, clustersVec[elemIt.first], actionsVec[elemIt.first]);

	//reset sequence modified
	resetModified();

	return true;
}

void EditorMultiSequence::clear()
{
	for (auto& video : m_Videos)
		video.reset();

	m_Videos.clear();
	m_VideosFramesSizes.clear();
	//m_FramesTimes.clear();

	EditorSequence::clear();
}

size_t EditorMultiSequence::getNumFrames() const
{
	if (m_VideosFramesSizes.empty())
		return 0;
	return m_VideosFramesSizes.back();
}

cv::Mat EditorMultiSequence::getFrameImage(size_t i_FrameNum) const
{
	if (i_FrameNum < getNumFrames())
	{
		auto it = std::lower_bound(m_VideosFramesSizes.begin(), m_VideosFramesSizes.end(), i_FrameNum + 1);
		size_t videoNr = it - m_VideosFramesSizes.begin();
		size_t realFrameNr;

		if (videoNr == 0)
			realFrameNr = i_FrameNum;
		else
			realFrameNr = i_FrameNum - m_VideosFramesSizes[videoNr - 1];

		if (videoNr < m_Videos.size() && m_Videos.at(videoNr))
			return m_Videos.at(videoNr)->getFrameImage(realFrameNr);
	}

	return cv::Mat();
}

std::vector<EditorClusterPtr> EditorMultiSequence::split(const EditorClusterPtr& i_Cluster, std::vector<size_t>& o_VideoIds) const
{
	EditorCluster in_cluster(*i_Cluster);

	int clusterId = in_cluster.getSource()->getClusterId();
	//printCluster(*i_Cluster, "Before SPLIT");

	for (size_t frame_nr : m_VideosFramesSizes)
		in_cluster.splitActions(frame_nr - 1);

	std::vector<size_t> actions_times;
	std::vector<EditorActionPtr> in_actions = in_cluster.getActions();
	std::vector<EditorClusterPtr> out_clusters;
	for (auto action : in_actions)
	{	
		int start_frame = action->getStartFrame();
		int end_frame = action->getEndFrame() + 1;

		//uzupelnij klatki w klustrze przy przecieciach clustra w miejscu akcji
		sequence::Cluster::FramesPositionsMap positions = in_cluster.getSource()->getFramesPositions();

		//insert start_frame
		if (action != in_actions.front())
		{
			cv::Rect rect = in_cluster.getSource()->getFrameAt(start_frame);
			positions.insert(std::make_pair(start_frame, cv::Point(rect.x, rect.y)));
		}

		//insert end_frame
		if (action != in_actions.back())
		{
			cv::Rect rect = in_cluster.getSource()->getFrameAt(end_frame);
			positions.insert(std::make_pair(end_frame, cv::Point(rect.x, rect.y)));
		}

		in_cluster.getSource()->setFramesPositions(positions);

		//create new cluster
		EditorClusterPtr newCluster = std::make_shared<EditorCluster>(in_cluster.getSource()->getClusterId());
		newCluster->getSource()->setFrameSize(in_cluster.getSource()->getFrameSize());
		newCluster->getSource()->setFramesPositions(sequence::Cluster::FramesPositionsMap());
		*newCluster->getAction() = *action;

		//dodaj cluster do wynikowej tablicy
		out_clusters.push_back(newCluster);

		//uzupelnij tablice przecie� akcji (potrzebna w dalszych obliczeniach)
		actions_times.push_back(end_frame);

		//dodaj informacje o numerze video w kt�rym znajduje si� cluster
		o_VideoIds.push_back(getSequenceNr(start_frame + 1));
	}

	//iterate through all frame positions
	sequence::Cluster::FramesPositionsMap framesPositions = in_cluster.getSource()->getFramesPositions();
	for (auto& framePos : framesPositions)
	{
		int actionId = std::lower_bound(actions_times.begin(), actions_times.end(), framePos.first + 1) - actions_times.begin();
		int videoId = o_VideoIds.at(actionId);
		int realFrameNr = (videoId != 0) ? (framePos.first - m_VideosFramesSizes[videoId - 1]) : framePos.first;

		//insert new position
		ClusterPtr c = out_clusters.at(actionId)->getSource();
		sequence::Cluster::FramesPositionsMap positions = c->getFramesPositions();
		positions.insert(std::make_pair(realFrameNr, framePos.second));
		c->setFramesPositions(positions);
	}

	/*
	std::cout << "AFTER SPLIT" << std::endl;
	int i = 1;
	for (auto c : out_clusters) {
		std::cout << " Cluster " << i << " " << std::endl;
		printCluster(*c);
		i++;
	}
	*/

	//sprawdz czy suma klatek z wszystkich klastri r�wna jest ilo�ci klatek w klastrze wej�ciowym
	assert( std::accumulate(out_clusters.begin(), out_clusters.end(), 0, [](const int& sum, EditorClusterPtr clu) { return int(sum + clu->getLength()); }) == in_cluster.getLength());

	return out_clusters;
}

EditorClusterPtr EditorMultiSequence::merge( std::vector<EditorClusterPtr>& i_Clusters,  std::vector<size_t>& i_VideoIds) const
{
	assert(!i_Clusters.empty());
	assert(i_Clusters.size() == i_VideoIds.size());

	//if (i_Clusters.size() == 1)
	//	return i_Clusters.front();

	//uzupe�nij dziury w klastrach akcjami none


	EditorClusterPtr out_cluster;

	/*
	std::cout << "BEFORE MERGE" << std::endl;
	int i = 1;
	for (auto c : i_Clusters) {
		std::cout << " Cluster " << i << " " << std::endl;
		printCluster(*c);
		i++;
	}
	*/

	//set out_cluster params
	EditorClusterPtr firstCluster = i_Clusters.front();
	out_cluster = std::make_shared<EditorCluster>(firstCluster->getSource()->getClusterId());
	out_cluster->getSource()->setFrameSize(firstCluster->getSource()->getFrameSize());
	*out_cluster->getAction(0) = *firstCluster->getAction();

	bool _print = firstCluster->getSource()->getClusterId() == 0;
	if (_print)
	{
		std::cout << "BEFORE MERGE" << std::endl;
		int i = 1;
		for (auto c : i_Clusters) {
			std::cout << " Cluster " << i << " " << std::endl;
			printCluster(*c);
			i++;
		}
	}

	//set out_cluster frames
	sequence::Cluster::FramesPositionsMap out_frames;
	for (int i = 0; i < i_Clusters.size(); i++)
	{
		EditorClusterPtr cluster = i_Clusters[i];
		int frameOffset = (i_VideoIds[i] == 0) ? 0 : m_VideosFramesSizes[i_VideoIds[i] - 1];

		for (auto frame : cluster->getSource()->getFramesPositions())
			out_frames.insert(std::make_pair(frame.first + frameOffset, frame.second));
	}
	
	//Check if frames are not from interpolating funcion, otherwise remove duplicate values
	Cluster testCluster;
	auto it = out_frames.begin();
	while (it != out_frames.end())
	{
		//pomin pierwszy i ostatni element
		if (it->first == out_frames.begin()->first || it->first == out_frames.rbegin()->first)
		{
			++it;
			continue;
		}

		//zachowaj sprawdzan� klatk�
		auto testFrame = *it;

		//wyczy�� sprawdzan� klatk� z mapy klatek
		it = out_frames.erase(it);

		//ustaw klatki w clustrze
		testCluster.setFramesPositions(out_frames);

		//pobierz warto�� prostok�ta z czasu sprawdzanej klatki
		cv::Rect testRect = testCluster.getFrameAt(testFrame.first);

		//je�li pobrana warto�� jest r�wna warto�ci testowanej kaltki, oznacza to �e klatka jest zb�dna , w przeciwnym wypadku dodaj j� spowrotem do mapy klatek
		if (testFrame.second != cv::Point(testRect.x, testRect.y))
		{
			it = out_frames.insert( testFrame).first;
			++it;
		}
	}
	out_cluster->getSource()->setFramesPositions(out_frames);
	out_cluster->update();

	//split actions
	for (int i = 0; i < i_Clusters.size(); i++)
	{
		EditorClusterPtr cluster = i_Clusters[i];
		if (cluster != i_Clusters.front())
		{
			EditorActionPtr action = cluster->getAction();

			//sprawdz czy jest ci�g�o�c z poprzedni� akcj�, je�li nie uzupe�nij o akcje z nieprzypisanym id
			EditorActionPtr prevAction = i_Clusters[i - 1]->getAction();

			int currentAndLastActionVideoFrameOffset = m_VideosFramesSizes[i_VideoIds[i]] - m_VideosFramesSizes[i_VideoIds[i-1]];
			if (prevAction->getEndFrame() != (action->getStartFrame() + currentAndLastActionVideoFrameOffset - 1) )
			{
				int frameOffset = (i_VideoIds[i] == 0) ? 0 : m_VideosFramesSizes[i_VideoIds[i] - 1];
				out_cluster->splitActions(prevAction->getEndFrame());
				//reset action
				out_cluster->getActions().back()->clear();
			}

			EditorActionPtr lastAction = out_cluster->getActions().back();

			//jesli akcje s� identyczne i s� na z��czeniu dw�ch sekwencji to nie dziel ich
			bool isActionContinuation = (action->getSource()->getActionId() == lastAction->getSource()->getActionId() &&
				action->getStartFrame() == 0);

			if (!isActionContinuation)
			{
				int frameOffset = (i_VideoIds[i] == 0) ? 0 : m_VideosFramesSizes[i_VideoIds[i] - 1];
				int realFrameNr = action->getStartFrame() + frameOffset;

				out_cluster->splitActions(realFrameNr - 1);
				*out_cluster->getActions().back()->getSource() = *action->getSource();
			}
		}
	}

	if (_print)
	  printCluster(*out_cluster, "AFTER MERGE");

	return out_cluster;
}


void EditorMultiSequence::fixCluster(EditorClusterPtr i_Cluster)
{
	assert(i_Cluster);
	
	bool needUpdate = false;

	if (i_Cluster->getSource()->getClusterId() == 0)
		qDebug() << m_VideosFramesSizes.back();

	sequence::Cluster::FramesPositionsMap positions = i_Cluster->getSource()->getFramesPositions();
	for (auto it = positions.begin(); it != positions.end();)
	{
		if (it->first > m_VideosFramesSizes.back())
		{
			it = positions.erase(it);
			needUpdate = true;
		}
		else
		{
			++it;
		}
	}

	if (needUpdate) {
		i_Cluster->getSource()->setFramesPositions(positions);
	}
	
}

void EditorMultiSequence::printCluster(const EditorCluster& cluster, const std::string& info) const
{
	ClusterPtr clu = cluster.getSource();

	std::cout << "--------------- " << info << " -------------------------" << std::endl;
	std::cout << "[Cluster id] " << clu->getClusterId() << std::endl;
	std::cout << "[BeginFrame] " << clu->getStartFrame() << std::endl;
	std::cout << "[EndFrame] " << clu->getEndFrame() << std::endl;

	std::cout << "[Frames] " << clu->getFramesPositions().size() << std::endl;
	for (auto& frame : clu->getFramesPositions())
		std::cout << "  " <<frame.first << " (" << frame.second.x << "," << frame.second.y << ")" << std::endl;

	std::cout << "[Actions] " << cluster.getActions().size() <<  std::endl;
	for (auto action: cluster.getActions() )
		std::cout << "  (" << action->getStartFrame() << ", " << action->getEndFrame() << ")" << " id: " << action->getSource()->getActionId() << std::endl;

	std::cout << std::endl;
}

bool EditorMultiSequence::setClusterStartFrame(size_t i_ClusterId, size_t i_StartFrameNr)
{
	int currentFrame = EditorTimeline::getInstance().getCurrentFrame();
	if (std::find(m_VideosFramesSizes.begin(), m_VideosFramesSizes.end(), currentFrame+1) == m_VideosFramesSizes.end())
		return EditorSequence::setClusterStartFrame(i_ClusterId, i_StartFrameNr);	

	return false;
}

bool EditorMultiSequence::setClusterEndFrame(size_t i_ClusterId, size_t i_EndFrameNr)
{
	int currentFrame = EditorTimeline::getInstance().getCurrentFrame();
	if (std::find(m_VideosFramesSizes.begin(), m_VideosFramesSizes.end(), currentFrame+1) == m_VideosFramesSizes.end())
		return EditorSequence::setClusterEndFrame(i_ClusterId, i_EndFrameNr);

	return false;
}

int EditorMultiSequence::getSequenceNr(size_t frameNr) const
{
	if (frameNr < getNumFrames())
		return std::lower_bound(m_VideosFramesSizes.begin(), m_VideosFramesSizes.end(), frameNr + 1) - m_VideosFramesSizes.begin();

	return -1;
}
