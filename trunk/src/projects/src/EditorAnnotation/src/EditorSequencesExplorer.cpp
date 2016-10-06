#include "EditorSequencesExplorer.h"


const QString EditorSequencesExplorer::SEQUENCE_EXTENSIONS = "*.vsq;*.cvs;*.seq";
const QString EditorSequencesExplorer::CLUSTER_EXTENSION = "clu";
const QString EditorSequencesExplorer::ACTION_EXTENSION = "acn";

/*--------------------------------------------------------------------*/
bool EditorSequencesExplorer::load(const std::string& i_WorkingDir)
{
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
	//get all sequence files
	QFileInfoList sequenceFiles = qDir.entryInfoList(SEQUENCE_EXTENSIONS.split(";"), QDir::Files);
	foreach(QFileInfo file, sequenceFiles)
		m_SeqFileNames.push_back(file.fileName().toStdString());
	
	//sort sequences files
	std::sort(m_SeqFileNames.begin(), m_SeqFileNames.end(), FileNameCompare());

	//CLUSTER FILES
	//create cluster action map
	std::map<std::string, std::string> l_PrefixMap;
	for (auto it = m_SeqFileNames.begin(); it != m_SeqFileNames.end(); ++it) {

		m_ClusterMap[*it] = std::vector<std::string>();
		l_PrefixMap[QFileInfo(it->c_str()).completeBaseName().toStdString()] = *it;
	}

	//get all cluster files
	QFileInfoList clusterFiles = qDir.entryInfoList(QStringList() << ("*." + CLUSTER_EXTENSION), QDir::Files);
	foreach(QFileInfo file, clusterFiles){

		QString baseFile = file.completeBaseName();
		QString prefixName = baseFile.left(baseFile.lastIndexOf("."));

		auto found = l_PrefixMap.find(prefixName.toStdString());
		if (found != l_PrefixMap.end())
			m_ClusterMap[found->second].push_back(file.fileName().toStdString());
		
	}

	//sort cluster
	for (auto it = m_SeqFileNames.begin(); it != m_SeqFileNames.end(); ++it)
		std::sort(m_ClusterMap[*it].begin(), m_ClusterMap[*it].end(), FileNameCompare());


	//ACTION FILES
	//check action files
	for (auto it = m_ClusterMap.begin(); it != m_ClusterMap.end(); ++it){
		for (auto cl_it = it->second.begin(); cl_it != it->second.end(); ++cl_it) {

			QString actionFile = QString(cl_it->c_str()).replace(CLUSTER_EXTENSION, ACTION_EXTENSION);
			m_ClusterActionMap[*cl_it] = qDir.exists(actionFile) ? actionFile.toStdString() : "";
		}
	}

	//debugPrint();
	return true;
}

/*--------------------------------------------------------------------*/
bool EditorSequencesExplorer::reload(){
	return load(m_WorkingDir);
}

/*--------------------------------------------------------------------*/
void EditorSequencesExplorer::reloadSequenceFiles(const std::string& i_SeqName)
{
	//check if file exists
	auto foundIt = std::find(m_SeqFileNames.begin(), m_SeqFileNames.end(), i_SeqName);
	if (foundIt == m_SeqFileNames.end())
		return;

	//clear all dependencies
	std::vector<std::string> clusters = getSequenceClusters(i_SeqName);
	for (std::string clusterFile : clusters)
	{
		auto found = m_ClusterActionMap.find(clusterFile);
		if (found != m_ClusterActionMap.end())
			m_ClusterActionMap.erase(found);
	}

	auto found = m_ClusterMap.find(i_SeqName);
	if (found != m_ClusterMap.end())
		found->second.clear();

	//insert cluster filse
	QDir qDir(m_WorkingDir.c_str());
	QFileInfoList clusterFiles = qDir.entryInfoList(QStringList() << ("*." + CLUSTER_EXTENSION), QDir::Files);
	foreach(QFileInfo file, clusterFiles){

		QString baseFile = file.completeBaseName();
		QString prefixName = baseFile.left(baseFile.lastIndexOf("."));

		if (prefixName == QFileInfo(i_SeqName.c_str()).completeBaseName())
			m_ClusterMap[i_SeqName].push_back(file.fileName().toStdString());
	}

	std::sort(m_ClusterMap[i_SeqName].begin(), m_ClusterMap[i_SeqName].end(), FileNameCompare());

	//insert action files
	for (auto it = m_ClusterMap[i_SeqName].begin(); it != m_ClusterMap[i_SeqName].end(); ++it) {
		QString actionFile = QString(it->c_str()).replace(CLUSTER_EXTENSION, ACTION_EXTENSION);
		m_ClusterActionMap[*it] = qDir.exists(actionFile) ? actionFile.toStdString() : "";
	}
}

/*--------------------------------------------------------------------*/
std::vector<std::string> EditorSequencesExplorer::getSequenceClusters(const std::string& i_SeqFileName) const
{
	auto foundIt = m_ClusterMap.find(i_SeqFileName);
	return (foundIt != m_ClusterMap.end()) ? foundIt->second : std::vector<std::string>();
}

/*--------------------------------------------------------------------*/
std::vector<std::string> EditorSequencesExplorer::getSequenceActions(const std::string& i_SeqFileName) const
{
	std::vector<std::string> l_Actions;
	std::vector<std::string> l_SeqClusters = getSequenceClusters(i_SeqFileName);
	for (std::string clusterFile : l_SeqClusters){
		std::string actionFile = getActionByCluster(clusterFile);
		if (!actionFile.empty())
			l_Actions.push_back(actionFile);
	}

	return l_Actions;
}

/*--------------------------------------------------------------------*/
std::string EditorSequencesExplorer::getActionByCluster(const std::string& i_ClusterFileName) const
{
	auto foundIt = m_ClusterActionMap.find(i_ClusterFileName);
	return (foundIt != m_ClusterActionMap.end()) ? foundIt->second : std::string();
}

/*--------------------------------------------------------------------*/
std::string EditorSequencesExplorer::getAbsolutePath(const std::string& i_FileName) const
{
	return m_WorkingDir + "/" + i_FileName;
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
void EditorSequencesExplorer::clear()
{
	m_WorkingDir = "";
	m_SeqFileNames.clear();
	m_ClusterMap.clear();
	m_ClusterActionMap.clear();
}


/*--------------------------------------------------------------------*/
int EditorSequencesExplorer::convertFileNameToInt(const std::string& i_FileName)
{
	QStringList parts = QString(i_FileName.c_str()).split(".");
	for (int i = parts.size() - 1; i >= 0; i--)
	{
		bool ok;
		int number = parts[i].toInt(&ok);
		if (ok)
			return number;
	}

	return -1;
}

/*--------------------------------------------------------------------*/
void EditorSequencesExplorer::debugPrint()
{
	std::cout << "Working dir: " << m_WorkingDir << std::endl;

	for (auto it = m_SeqFileNames.begin(); it != m_SeqFileNames.end(); ++it){
		std::cout << "------------- Sequence: " << *it << std::endl;
		std::vector<std::string> clusters = m_ClusterMap[*it];
		for (auto jt = clusters.begin(); jt != clusters.end(); ++jt)
		{
			std::cout << *jt << "   " << m_ClusterActionMap[*jt] << std::endl;
		}
	}
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
