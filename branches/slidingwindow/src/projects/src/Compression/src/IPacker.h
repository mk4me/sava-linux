#pragma once
#ifndef IPacker_h__
#define IPacker_h__

#include <sequence/IVideo.h>

#include <opencv2/core.hpp>

/// <summary>
/// interfejs pozwalaj¹cy na tworzenie ró¿nych implementacji procesu kompresji. Wybór metody dokonywany jest w konfiguracji.
/// </summary>
class IPacker
{
public:
	/// <summary>
	/// Creates the sequence.
	/// </summary>
	virtual void createSequence() = 0;
	/// <summary>
	/// Compresses the frame.
	/// </summary>
	/// <param name="frameId">The frame identifier.</param>
	/// <param name="frame">The frame.</param>
	/// <param name="timestamp">The timestamp.</param>
	virtual void compressFrame(size_t frameId, const cv::Mat& frame, sequence::IVideo::Timestamp timestamp) = 0;
	/// <summary>
	/// Saves the specified file.
	/// </summary>
	/// <param name="filename">The filename.</param>
	virtual void save(const std::string& filename) = 0;
	virtual std::shared_ptr<sequence::IVideo> getVideo() = 0;
};

#endif // IPacker_h__
