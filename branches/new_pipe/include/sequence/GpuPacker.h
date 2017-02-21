#pragma once
#ifndef GpuPacker_h__
#define GpuPacker_h__

#include "IPacker.h"
#include <sequence/BackgroundSeparation.h>
namespace cv
{
	class BackgroundSubtractor;
}

namespace sequence
{
	class GpuVideo;
}

/// <summary>
/// klasa implementuj¹ca interfejs IPacker. Tworzy wyjciow¹ sekwencje przechowuj¹c¹ ca³e obrazy ale zoptymalizowane pod k¹tem szybkoci dekompresji. U¿ywa do tego zewnêtrznej biblioteki LibGpuJpeg.
/// </summary>
/// <seealso cref="IPacker" />
class GpuPacker : public IPacker
{
public:
	GpuPacker(int imageCompression, int backgroundHistory = 300, int differenceThreshold = 20, float newBackgroundMinPixels = 0.2, int minCrumbleArea = 100, int mergeCrumblesIterations = 3, const cv::Mat& mask = cv::Mat());

	/// <summary>
	/// Creates the sequence.
	/// </summary>
	virtual void createSequence() override;
	/// <summary>
	/// Compresses the frame.
	/// </summary>
	/// <param name="frameId">The frame identifier.</param>
	/// <param name="frame">The frame.</param>
	/// <param name="timestamp">The timestamp.</param>
	virtual void compressFrame(size_t frameId, const cv::Mat& frame, sequence::IVideo::Timestamp timestamp) override;
	/// <summary>
	/// Saves the specified file.
	/// </summary>
	/// <param name="filename">The filename.</param>
	virtual void save(const std::string& filename) override;

private:
	std::shared_ptr<sequence::GpuVideo> m_GpuVideo;
	//cv::Ptr<cv::BackgroundSubtractor> m_BackgroundSubtractor;
	//cv::Mat m_LastBackground;
	size_t m_BackgroundFrame;

	int m_ImageCompression;
	//int m_BackgroundHistory;	// 300
	//int m_DifferenceThreshold;	// 20
	//float m_NewBackgroundMinPixels;		// 0.2
	//int m_MinCrumbleArea;		// 100
	//int m_MergeCrumblesIterations;	// 3

	//cv::Mat m_CameraMask;
    sequence::FBSeparator m_separator;

	void resetBackgroundSubtractor();
};

#endif // GpuPacker_h__