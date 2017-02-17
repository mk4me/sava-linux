#pragma once

#include <utils/Singleton.h>
#include "EditorSingleSequence.h"
#include "EditorMultiSequence.h"
#include "EditorSequencesExplorer.h"

#include <QtCore/QThread>
#include <QtCore/QMutex>

#define VIDEOS_TIMESTAMPS_ENABLED 1

/// <summary>
/// Zarz¹dza wczytywaniem nowych sekwencji i zapisywaniem bie¿¹cych.
/// </summary>
/// <seealso cref="Singleton{EditorSequencesManager}" />
class EditorSequencesManager : public QObject, public Singleton < EditorSequencesManager >
{

	Q_OBJECT

public:
	EditorSequencesManager();
	virtual ~EditorSequencesManager() { }

	/// <summary>
	/// Loads the videos.
	/// </summary>
	/// <param name="i_VideosPaths">The videos file paths.</param>
	/// <returns>success??</returns>
	void load(const std::vector<std::string>& i_VideosPaths);

	/// <summary>
	/// Saves this instance.
	/// </summary>
	void save() const;


	/// <summary>
	/// Loads the next sequence.
	/// </summary>
	/// <param name="frameNr">The next frame nr in loaded sequence (-1 means last frame of sequence).</param>
	/// <returns></returns>
	bool loadNext(int frameNr = 0);


	/// <summary>
	/// Loads the previous.
	/// </summary>
	/// <param name="frameNr">TThe next frame nr in loaded sequence (-1 means last frame of sequence).</param>
	/// <returns></returns>
	bool loadPrev(int frameNr = 0);

	/// <summary>
	/// Determines whether this video is loaded.
	/// </summary>
	/// <returns>is loaded?</returns>
	bool isLoaded() const;

	/// <summary>
	/// Gets current sequence.
	/// </summary>
	/// <returns>EditorSequencePtr.</returns>
	EditorSequencePtr getSequence() { return m_Sequence; }


	/// <summary>
	/// Gets current sequences explorer.
	/// </summary>
	EditorSequencesExplorerPtr getExplorer() { return m_Explorer; }


	std::vector<std::string> getVideosPaths() const { return m_VideosPaths; };

	/// <summary>
	/// Create unique cluster id.
	/// </summary>
	/// <returns>Unique cluster id.</returns>
	static int createUniqueClusterId();

#if VIDEOS_TIMESTAMPS_ENABLED
	sequence::IVideo::Timestamp getVideoFirstTime(const std::string& seqVideoFile) const;
	sequence::IVideo::Timestamp getVideoLastTime(const std::string& seqVideoFile) const;
#endif

signals:
	void loadingStarted();
	void loadingFinished(bool loadSuccess);

private slots:
	void loadingFinished_internal();
	void loadingStarted_internal();

private:
	bool loadInternal(const std::vector<std::string>& i_VideosPaths);

	//try load sequence explorer
	bool tryLoadExplorer(const std::string& i_FilePathFromExplorerDir);

	void onExplorerLoaded();

	//create sequence by type
	EditorSequencePtr createSequenceByType(EEditorSequenceType _type) const;
	
	template <typename T>
	std::shared_ptr<T> castSequence(const EditorSequencePtr& _castSeq) const;

private:
	//unique cluste id
	static int m_UniqueClusterId;

	//current sequence
	EditorSequencePtr m_Sequence;

	/// File manager
	EditorSequencesExplorerPtr m_Explorer;

	//video files paths
	std::vector<std::string> m_VideosPaths;

	int m_LoadFrameNr;
	bool m_WasPlaying;
	bool m_IsLoading;
	QMutex m_LoadMutex;

#if VIDEOS_TIMESTAMPS_ENABLED
	//start - end videos times
	std::map<std::string, std::pair<sequence::IVideo::Timestamp, sequence::IVideo::Timestamp>> m_VideosTimestamps;
#endif



private:
	class LoadThread : public QThread
	{

	public:
		LoadThread() : QThread(), m_LoadSuccess(false) {}

		void setFilesToLoad(const std::vector<std::string>& files) { m_Files = files; }
		bool isLoadSuccess() const { return m_LoadSuccess; }
		virtual void run() override;

	private:
		std::vector<std::string> m_Files;
		bool m_LoadSuccess;
	}
	m_LoadThread;
};



