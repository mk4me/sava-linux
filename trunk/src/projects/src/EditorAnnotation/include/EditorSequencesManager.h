#pragma once

#include <utils/Singleton.h>
#include "EditorSequence.h"
#include "EditorSequencesExplorer.h"

class EditorSequencesManager: public Singleton<EditorSequencesManager>
{
public:
	EditorSequencesManager();
	virtual ~EditorSequencesManager();


	/// <summary>
	/// Loads the video.
	/// </summary>
	/// <param name="i_VideoFilePath">The video file path.</param>
	/// <returns>success??</returns>
	bool load(const std::string& i_VideoFilePath);

	/// <summary>
	/// Saves this instance.
	/// </summary>
	void save() const;

	/// <summary>
	/// Loads next video.
	/// </summary>
	/// <returns>success??</returns>
	bool loadNext();

	/// <summary>
	/// Loads previous video.
	/// </summary>
	/// <returns>success??</returns>
	bool loadPrev();

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
	/// Gets sequences dir explorer.
	/// </summary>
	/// <returns>EditorSequencesExplorerPtr.</returns>
	EditorSequencesExplorerPtr getExplorer() { return m_Explorer; }


private:
	//load seq
	bool loadSequence(const std::string& i_VidoePath);

	//get next video file name
	std::string getNextVideoFile() const;

	//get prev video file name
	std::string getPrevVideoFile() const;

	//current sequence
	EditorSequencePtr m_Sequence;

	//sequences file explorer
	EditorSequencesExplorerPtr m_Explorer;
};

