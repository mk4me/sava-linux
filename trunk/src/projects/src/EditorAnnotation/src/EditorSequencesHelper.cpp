#include "EditorSequencesHelper.h"

//--------------------------------------------------
EditorSequencesHelper::EditorSequencesHelper(){
}

//--------------------------------------------------
EditorSequencesHelper::~EditorSequencesHelper(){
}

//--------------------------------------------------
bool EditorSequencesHelper::load(const std::string i_Dir)
{
	if (i_Dir.empty())
		return false;

	QDir qDir(i_Dir.c_str());
	if (!qDir.exists())
		return false;

	m_Dir = i_Dir;
	m_Files.clear();

	QFileInfoList files = qDir.entryInfoList(QStringList() << "*.csq", QDir::NoDotAndDotDot | QDir::Files);
	foreach(QFileInfo file, files){
		m_Files.push_back(file.absoluteFilePath().toStdString());
	}

	if (m_Files.empty())
		return false;

	FileNameComparator comparator;
	std::sort(m_Files.begin(), m_Files.end(), comparator);

	return true;
}
