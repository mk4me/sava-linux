#include "EditorSequencesExplorer.h"
#include "boost/timer/timer.hpp"
#include <assert.h>

const QString EditorSequencesExplorer::SEQUENCE_EXTENSIONS = "*.vsq;*.cvs;";
const QString EditorSequencesExplorer::CLUSTER_EXTENSION = "clu";
const QString EditorSequencesExplorer::ACTION_EXTENSION = "acn";
const QString EditorSequencesExplorer::COMPLEX_ACTION_EXTENSION = "can";

#ifdef _DEBUG
	#define PROFILING_ENABLED 1
#else
	#define PROFILING_ENABLED 0
#endif // DEBUG



#if PROFILING_ENABLED
	#define INIT_PROFILE boost::timer::cpu_timer timer;
	#define START_PROFILE(info) std::cout << info; timer.start();
	#define STOP_PROFILE  std::cout << timer.elapsed().wall / 1000000 << std::endl;
#else
	#define INIT_PROFILE
	#define START_PROFILE(info)
	#define STOP_PROFILE
#endif

/*--------------------------------------------------------------------*/
bool EditorSequencesExplorer::load(const std::string& i_WorkingDir)
{
	INIT_PROFILE

	if (i_WorkingDir.empty())
		return false;

	//get all files from working dir
	QDir qDir(i_WorkingDir.c_str());

	//if wrong path return false
	if (!qDir.exists())
		return false;

	//clear all fields
	clear();

	m_WorkingDir = i_WorkingDir;


	//SEQTION FILES
	START_PROFILE("Getting video list .... ")

	QFileInfoList seqList = qDir.entryInfoList(SEQUENCE_EXTENSIONS.split(";"), QDir::Files);
	if (seqList.empty())
		return true;

	m_SeqFileNames.reserve(seqList.size());
	foreach(QFileInfo _info, seqList) 
	{
		std::string seqFileName = _info.fileName().toStdString();
		m_SeqFileNames.push_back(seqFileName);
		m_ClusterMap.insert(std::make_pair(seqFileName, std::vector<std::string>()));
		m_ComplexActionMap.insert(std::make_pair(seqFileName, std::vector<std::string>()));
	}

	//sort sequences
	std::sort(m_SeqFileNames.begin(), m_SeqFileNames.end(), FileNameCompare());

	QString seqExt = seqList.at(0).suffix();

	STOP_PROFILE



	//CLUSTER FILES
	START_PROFILE("Getting cluster list .... ")

	QFileInfoList cluList = qDir.entryInfoList(QStringList() << ("*." + CLUSTER_EXTENSION), QDir::Files, QDir::Name | QDir::Reversed);

	QString currentClu_Seq;
	std::map<std::string, std::vector<std::string>>::iterator currentClu_It = m_ClusterMap.end();
	foreach(QFileInfo _info, cluList)
	{
		QString cluSeq = _info.completeBaseName();
		cluSeq = cluSeq.left(cluSeq.lastIndexOf(".")) + "." + seqExt;

		if (currentClu_Seq != cluSeq){
			currentClu_Seq = cluSeq;
			currentClu_It = m_ClusterMap.find(currentClu_Seq.toStdString());
		}

		if (currentClu_It != m_ClusterMap.end()) {
			currentClu_It->second.push_back(_info.fileName().toStdString());
		}
	}

	//sort cluster
	for (auto it = m_SeqFileNames.begin(); it != m_SeqFileNames.end(); ++it)
		std::sort(m_ClusterMap[*it].begin(), m_ClusterMap[*it].end(), FileNameCompare());

	STOP_PROFILE


	START_PROFILE("Getting action list ... ")

	QFileInfoList actionList = qDir.entryInfoList(QStringList() << ("*." + ACTION_EXTENSION), QDir::Files);
	foreach(QFileInfo _info, actionList)
	{
		//cluster file must exists
		QString actionFile = _info.fileName();
		QString clusterFile(actionFile);
		clusterFile.replace(ACTION_EXTENSION, CLUSTER_EXTENSION);

		m_ClusterActionMap.insert(std::make_pair(clusterFile.toStdString(), actionFile.toStdString()));
	}

	STOP_PROFILE


	START_PROFILE("Getting complex action list ...")

	QFileInfoList complexList = qDir.entryInfoList(QStringList() << ("*." + COMPLEX_ACTION_EXTENSION), QDir::Files);

	QString currentComplex_Seq;
	std::map<std::string, std::vector<std::string>>::iterator currentComplex_It = m_ComplexActionMap.end();
	foreach(QFileInfo _info, complexList)
	{
		QString cluSeq = _info.completeBaseName();
		cluSeq = cluSeq.left(cluSeq.lastIndexOf(".")) + "." + seqExt;

		if (currentComplex_Seq != cluSeq){
			currentComplex_Seq = cluSeq;
			currentComplex_It = m_ComplexActionMap.find(currentComplex_Seq.toStdString());
		}

		if (currentComplex_It != m_ComplexActionMap.end()) {
			currentComplex_It->second.push_back(_info.fileName().toStdString());
		}
	}

	STOP_PROFILE

	//debugToFile("Info.txt");

	return true;
}


/*--------------------------------------------------------------------*/
bool EditorSequencesExplorer::reload(){
	return load(m_WorkingDir);
}


/*--------------------------------------------------------------------*/
std::vector<std::string> EditorSequencesExplorer::getSequenceClusters(const std::string& i_SeqFileName) const
{
	auto foundIt = m_ClusterMap.find(i_SeqFileName);
	return (foundIt != m_ClusterMap.end()) ? foundIt->second : std::vector<std::string>();
}

/*--------------------------------------------------------------------*/
std::vector<std::string> EditorSequencesExplorer::getSequenceComplexActions(const std::string& i_SeqFileName) const
{
	auto foundIt = m_ComplexActionMap.find(i_SeqFileName);
	return (foundIt != m_ComplexActionMap.end()) ? foundIt->second : std::vector<std::string>();
}


/*--------------------------------------------------------------------*/
std::vector<std::string> EditorSequencesExplorer::getSequenceActions(const std::string& i_SeqFileName) const
{
	std::vector<std::string> actions;
	std::vector<std::string> clusters = getSequenceClusters(i_SeqFileName);
	for (std::string cluster : clusters)
	{
		auto it = m_ClusterActionMap.find(cluster);
		if (it != m_ClusterActionMap.end())
			actions.push_back(it->second);
	}

	return actions;
}


/*--------------------------------------------------------------------*/
std::string EditorSequencesExplorer::getClusterAction(const std::string& i_ClusterFileName) const
{
	auto foundIt = m_ClusterActionMap.find(i_ClusterFileName);
	return (foundIt != m_ClusterActionMap.end()) ? foundIt->second : std::string();
}

/*--------------------------------------------------------------------*/
std::string EditorSequencesExplorer::getAbsolutePath(const std::string& i_FileName) const{
	return m_WorkingDir + "/" + i_FileName;
}

/*--------------------------------------------------------------------*/
std::string EditorSequencesExplorer::getRelativePath(const std::string& i_FilePath) const{
	return i_FilePath.substr(m_WorkingDir.size() + 1);
}


/*--------------------------------------------------------------------*/
void EditorSequencesExplorer::refreshClustersAndActions(const std::string& i_SeqFileName, std::vector<std::string>& i_Clusters, std::vector<std::string>& i_Actions)
{
	assert(i_Clusters.size() == i_Actions.size());

	std::vector<std::string> clusters = getSequenceClusters(i_SeqFileName);
	std::vector<std::string> actions = getSequenceActions(i_SeqFileName);


	//remove from clusterActionMap
	for (std::string elem : clusters)
	{
		auto foundIt = m_ClusterActionMap.find(elem);
		if (foundIt != m_ClusterActionMap.end())
			m_ClusterActionMap.erase(foundIt);
	}

	//insert to clusterActionMap
	for (int i = 0; i < i_Clusters.size(); i++)
	{
		if (!i_Actions[i].empty())
			m_ClusterActionMap.insert(std::make_pair(i_Clusters[i], i_Actions[i]));
	}

	std::vector<std::string> clustersToDelete;
	std::vector<std::string> actionsToDelete;

	std::sort(clusters.begin(), clusters.end());
	std::sort(i_Clusters.begin(), i_Clusters.end());
	std::set_difference(clusters.begin(), clusters.end(), i_Clusters.begin(), i_Clusters.end(), std::back_inserter(clustersToDelete));

	std::sort(actions.begin(), actions.end());
	std::sort(i_Actions.begin(), i_Actions.end());
	std::set_difference(actions.begin(), actions.end(), i_Actions.begin(), i_Actions.end(), std::back_inserter(actionsToDelete));

	for (std::string file : clustersToDelete)
		deleteFile(file);

	for (std::string file : actionsToDelete)
		deleteFile(file);


	//insert to cluster map
	auto clusterIt = m_ClusterMap.find(i_SeqFileName);
	if (clusterIt != m_ClusterMap.end())
	{
		clusterIt->second.clear();
		clusterIt->second = i_Clusters;
		std::sort(clusterIt->second.begin(), clusterIt->second.end(), FileNameCompare());
	}
}

/*--------------------------------------------------------------------*/
void EditorSequencesExplorer::refreshComplexActions(const std::string& i_SeqFileName, std::vector<std::string> i_ComplexActions)
{
	auto mapIt = m_ComplexActionMap.find(i_SeqFileName);
	if (mapIt == m_ComplexActionMap.end())
		return;

	std::vector<std::string> actions = getSequenceComplexActions(i_SeqFileName);
	std::vector<std::string> actionsToDelete;
	
	std::sort(actions.begin(), actions.end());
	std::sort(i_ComplexActions.begin(), i_ComplexActions.end());
	std::set_difference(actions.begin(), actions.end(), i_ComplexActions.begin(), i_ComplexActions.end(), std::back_inserter(actionsToDelete));

	mapIt->second = i_ComplexActions;

	for (std::string file : actionsToDelete)
		deleteFile(file);
}

/*--------------------------------------------------------------------*/
std::string EditorSequencesExplorer::createFile(const std::string& i_FileName)
{
	std::string filePath = getAbsolutePath(i_FileName);
	QFile file(filePath.c_str());
	if (file.open(QIODevice::ReadWrite))
	{
		file.close();
		return filePath;
	}

	return std::string();
}


/*--------------------------------------------------------------------*/
void EditorSequencesExplorer::deleteFile(const std::string& i_FileName)
{
	std::string filePath = getAbsolutePath(i_FileName);
	QFile::remove(filePath.c_str());
}

/*--------------------------------------------------------------------*/
bool EditorSequencesExplorer::exists(const std::string& i_FileName)
{
	std::string fullFileName = getAbsolutePath(i_FileName);
	return QFile::exists(fullFileName.c_str());
}

/*--------------------------------------------------------------------*/
std::string EditorSequencesExplorer::getNextSequence(const std::string& i_SeqName)
{
	auto foundIt = std::find(m_SeqFileNames.begin(), m_SeqFileNames.end(), i_SeqName);
	if (foundIt != m_SeqFileNames.end() && (foundIt + 1) != m_SeqFileNames.end())
		return *(foundIt + 1);

	return std::string();
}

/*--------------------------------------------------------------------*/
std::string EditorSequencesExplorer::getPrevSequence(const std::string& i_SeqName)
{
	auto foundIt = std::find(m_SeqFileNames.begin(), m_SeqFileNames.end(), i_SeqName);
	if (foundIt != m_SeqFileNames.end() && foundIt != m_SeqFileNames.begin())
		return *(foundIt - 1);

	return std::string();
}

/*--------------------------------------------------------------------*/
void EditorSequencesExplorer::clear()
{
	m_WorkingDir.clear();
	m_SeqFileNames.clear();
	m_ClusterMap.clear();
}


/*--------------------------------------------------------------------*/
void EditorSequencesExplorer::debugToFile(const std::string& _fileName) const
{
	QFile file(_fileName.c_str());
	file.open(QFile::WriteOnly | QFile::Text);

	QTextStream out(&file);

	for (auto it = m_ClusterMap.begin(); it != m_ClusterMap.end(); ++it)
	{
		QString seqName = it->first.c_str();
		out << seqName << "\n";

		std::vector<std::string> cluVec = it->second;
		for (auto v = cluVec.begin(); v != cluVec.end(); ++v) {

			std::string cluName = *v;
			std::string actName;
			auto actionIt = m_ClusterActionMap.find(cluName);
			if (actionIt != m_ClusterActionMap.end())
				actName = actionIt->second;

			out << "     " << cluName.c_str() << "  " << actName.c_str() << "\n";
		}
	}

	file.flush();
	file.close();

}


/*--------------------------------------------------------------------*/
int EditorSequencesExplorer::convertFileNameToInt(const std::string& i_FileName)
{
	QStringList parts = QString(i_FileName.c_str()).split(".");
	for (int i = parts.size() - 2; i >= 0; i--)
	{
		bool ok;
		int number = parts[i].toInt(&ok);
		if (ok)
			return number;
	}

	return -1;
}


/*--------------------------------------------------------------------*/
bool EditorSequencesExplorer::FileNameCompare::operator()(const std::string& i_FirstName, const std::string& i_SecondName)
{
	int firstInt = convertFileNameToInt(i_FirstName);
	int secondInt = convertFileNameToInt(i_SecondName);

	if (firstInt > -1 && secondInt > -1)
		return firstInt < secondInt;

	return i_FirstName < i_SecondName;
}


/*--------------------------------------------------------------------*/
std::vector<std::string> EditorSequencesExplorer::getSequencesFilesFromDir(const std::string& directoryPath)
{
	std::vector<std::string> seqences;

	QDir qDir(directoryPath.c_str());

	QFileInfoList seqList = qDir.entryInfoList(SEQUENCE_EXTENSIONS.split(";"), QDir::Files);
	if (!seqList.empty())
	{
		seqences.reserve(seqList.size());
		foreach(QFileInfo _info, seqList)
			seqences.push_back(_info.fileName().toStdString());

		std::sort(seqences.begin(), seqences.end(), FileNameCompare());
	}

	return seqences;
}
