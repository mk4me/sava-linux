#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <memory>

/// <summary>
/// klasa zajmuj¹ca analiz¹ wejciowego folderu z plikami wideo (.cvs, .vsq), plikami klastrów (.clu) oraz plikami akcji (.acn). 
/// Mo¿na siê z niej dowiedzieæ np. które pliki klastrów nale¿¹ do wybranej sekwencji wideo.
/// </summary>
class EditorSequencesExplorer
{
private:
	struct FileNameCompare {
		bool operator()(const std::string& i_FirstName, const std::string& i_SecondName);
	};

public:
	static const QString SEQUENCE_EXTENSIONS;
	static const QString CLUSTER_EXTENSION;
	static const QString ACTION_EXTENSION;
	static const QString COMPLEX_ACTION_EXTENSION;

	EditorSequencesExplorer() {}
	~EditorSequencesExplorer() {};

	
	/// <summary>
	/// Load the working dir.
	/// </summary>
	/// <param name="i_WorkingDir">The working dir full path.</param>
	/// <returns>success?</returns>
	bool load(const std::string& i_WorkingDir);

	/// <summary>
	/// Reloads workind dir.
	/// </summary>
	/// <returns>success?</returns>
	bool reload();

	/// <summary>
	/// Determines whether working dir is loaded.
	/// </summary>
	/// <returns>is loaded?</returns>
	bool isLoaded() const { return !m_WorkingDir.empty(); }

	/// <summary>
	/// Gets the working dir.
	/// </summary>
	/// <returns>Working dir</returns>
	std::string getWorkingDir() const { return m_WorkingDir; }

	/// <summary>
	/// Gets sequences list.
	/// </summary>
	/// <returns>vector of sequences file list</returns>
	std::vector<std::string> getSequences() const { return m_SeqFileNames; }

	/// <summary>
	/// Gets the clusters by sequence name.
	/// </summary>
	/// <param name="i_SeqFileName">Name of the sequence file.</param>
	/// <returns>Vector of cluster file names</returns>
	std::vector<std::string> getSequenceClusters(const std::string& i_SeqFileName) const;

	/// <summary>
	/// Gets the actions by sequence name.
	/// </summary>
	/// <param name="i_SeqFileName">Name of the sequence file.</param>
	/// <returns>Vector of action file names</returns>
	std::vector<std::string> getSequenceActions(const std::string& i_SeqFileName) const;

	/// <summary>
	/// Gets the complex actions by sequence name.
	/// </summary>
	/// <param name="i_SeqFileName">Name of the sequence file.</param>
	/// <returns>Vector of complex actions file names</returns>
	std::vector<std::string> getSequenceComplexActions(const std::string& i_SeqFileName) const;


	/// <summary>
	/// Gets the action by cluster.
	/// </summary>
	/// <param name="i_ClusterFileName">Name of the cluster file.</param>
	/// <returns>Action name file</returns>
	std::string getClusterAction(const std::string& i_ClusterFileName) const;

	/// <summary>
	/// Gets the absolute path.
	/// </summary>
	/// <param name="i_FileName">Name of the file.</param>
	/// <returns>Absolute path of file</returns>
	std::string getAbsolutePath(const std::string& i_FileName) const;

	/// <summary>
	/// Gets the relative path.
	/// </summary>
	/// <param name="i_FilePath">Full path of videos.</param>
	/// <returns>Relative path of file</returns>
	std::string getRelativePath(const std::string& i_FilePath) const;

	/// <summary>
	/// Refresh files in sequence.
	/// </summary>
	/// <param name="i_SeqFileName">Refreshed sequence.</param>
	/// <param name="i_Clusters">Clusters from sequence.</param>
	/// <param name="i_Actions">Actions from sequence.</param>
	void refreshClustersAndActions(const std::string& i_SeqFileName, std::vector<std::string>& i_Clusters, std::vector<std::string>& i_Actions);

	/// <summary>
	/// Refresh files in sequence.
	/// </summary>
	/// <param name="i_SeqFileName">Refreshed sequence.</param>
	/// <param name="i_ComplexActions">Complex actions from sequence.</param>
	void refreshComplexActions(const std::string& i_SeqFileName, std::vector<std::string> i_ComplexActions);

	/// <summary>
	/// Creates the file in loaded working dir.
	/// </summary>
	/// <param name="i_FileName">Name of the file.</param>
	/// <returns>Absolute path of file</returns>
	std::string createFile(const std::string& i_FileName);

	/// <summary>
	/// Deletes the file.
	/// </summary>
	/// <param name="i_FileName">Name of the file in loaded working dir.</param>
	void deleteFile(const std::string& i_FileName);

	bool exists(const std::string& i_FileName);

	std::string getNextSequence(const std::string& i_SeqName);
	std::string getPrevSequence(const std::string& i_SeqName);

	/// <summary>
	/// Try to convert the file name to int.
	/// </summary>
	/// <param name="i_FileName">Name of the file in loaded working dir.</param>
	/// <returns>Number of file name (cannot convert if return -1)</returns>
	static int convertFileNameToInt(const std::string& i_FileName);

	static std::vector<std::string> getSequencesFilesFromDir(const std::string& directoryPath);

private:
	void debugToFile(const std::string& _fileName) const;
	void clear();

	//current working dir
	std::string m_WorkingDir;

	//list of all seqences in working dir
	std::vector<std::string> m_SeqFileNames;

	// map< sequencesFile, vector<clusterFile> >
	std::map<std::string, std::vector<std::string>> m_ClusterMap;

	//map <clusterFile, actionFile>
	std::map<std::string, std::string> m_ClusterActionMap;

	//map <sequenceFile, vector<complexClusterFile>>
	std::map < std::string, std::vector<std::string>> m_ComplexActionMap;
};

typedef std::shared_ptr<EditorSequencesExplorer> EditorSequencesExplorerPtr;