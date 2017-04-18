#include "EditorSingleSequence.h"
#include "EditorSequencesManager.h"

/*--------------------------------------------------------------------*/
EditorSingleSequence::EditorSingleSequence()
	: EditorSequence()
	, m_Video(nullptr)
	, m_VideoPath("")
{

}

/*--------------------------------------------------------------------*/
void EditorSingleSequence::clear()
{
	m_Video.reset();
	m_VideoPath.clear();
	EditorSequence::clear();
}

/*--------------------------------------------------------------------*/
size_t EditorSingleSequence::getNumFrames() const 
{
	return m_Video ? m_Video->getNumFrames() : 0;
}


/*--------------------------------------------------------------------*/
cv::Mat EditorSingleSequence::getFrameImage(size_t i_FrameNum) const
{
	if (i_FrameNum < getNumFrames())
		return m_Video ? m_Video->getFrameImage(i_FrameNum) : cv::Mat();

	return cv::Mat();
}

std::vector<std::string> EditorSingleSequence::getVideosPaths() const
{
	return { m_VideoPath };
}

void EditorSingleSequence::setVideosPaths(const std::vector<std::string>& paths)
{
	assert(!paths.empty());
	m_VideoPath = paths.front();
}

