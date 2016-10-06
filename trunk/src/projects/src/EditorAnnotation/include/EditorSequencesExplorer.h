#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include <QtCore/QDir>
#include <memory>

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
	/// Reloads the sequence files.
	/// </summary>
	/// <param name="i_SeqName">Name of the sequence file.</param>
	void reloadSequenceFiles(const std::string& i_SeqName);

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
	/// Gets the sequence actions.
	/// </summary>
	/// <param name="i_SeqFileName">Name of the sequence file.</param>
	/// <returns>Vector of action file names</returns>
	std::vector<std::string> getSequenceActions(const std::string& i_SeqFileName) const;

	/// <summary>
	/// Gets the action by cluster.
	/// </summary>
	/// <param name="i_ClusterFileName">Name of the cluster file.</param>
	/// <returns>Action name file</returns>
	std::string getActionByCluster(const std::string& i_ClusterFileName) const;

	/// <summary>
	/// Gets the absolute path.
	/// </summary>
	/// <param name="i_FileName">Name of the file.</param>
	/// <returns>Absolute path of file</returns>
	std::string getAbsolutePath(const std::string& i_FileName) const;

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

	/// <summary>
	/// Try to convert the file name to int.
	/// </summary>
	/// <param name="i_FileName">Name of the file in loaded working dir.</param>
	/// <returns>Number of file name (cannot convert if return -1)</returns>
	static int convertFileNameToInt(const std::string& i_FileName);

private:
	void clear();
	void debugPrint();

	//current working dir
	std::string m_WorkingDir;

	//list of all seqences in working dir
	std::vector<std::string> m_SeqFileNames;

	// map< sequencesFile, vector<clusterFile> >
	std::map<std::string, std::vector<std::string>> m_ClusterMap;

	// map< clusterFile, actionFile >
	std::map<std::string, std::string> m_ClusterActionMap;
};

typedef std::shared_ptr<EditorSequencesExplorer> EditorSequencesExplorerPtr;