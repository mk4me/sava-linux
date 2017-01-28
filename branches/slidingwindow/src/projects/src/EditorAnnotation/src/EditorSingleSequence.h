#pragma once

#include "EditorSequence.h"
#include <QtCore/QFileInfo>

#include <map>

using namespace sequence;

/// <summary>
/// Klasa zawiera funkcjonalnoæ wczytywania, zarz¹dzania i przechowywania bie¿¹cej jednej sekwencji wideo wraz z jej klastrami i akcjami.
/// </summary>
/// <seealso cref="EditorSequence" />
class EditorSingleSequence : public EditorSequence
{

public:
	EditorSingleSequence();
	virtual ~EditorSingleSequence(){};

	EEditorSequenceType getType() const override { return EEditorSequenceType::SINGLE; }

	bool load() override { return false; };
	bool save() override { return false; };
	void clear() override;

	size_t getNumFrames() const override;
	cv::Mat getFrameImage(size_t i_FrameNum) const override;
	
	virtual std::vector<std::string> getVideosPaths() const override;
	virtual void setVideosPaths(const std::vector<std::string>& paths) override;

	virtual int getSequenceNr(size_t frameNr) const override { return 0; };

private:
	//current video 
	std::shared_ptr<IVideo> m_Video;

	//video path
	std::string m_VideoPath;
};
