#pragma once
#ifndef DefaultPacker_h__
#define DefaultPacker_h__

#include "IPacker.h"

namespace cv
{
	class BackgroundSubtractor;
}

namespace sequence
{
	class IVideo;
	class CompressedVideo;
}

/// <summary>
/// klasa implementuj¹ca interfejs IPacker. Tworzy wyjciow¹ sekwencjê skompresowan¹ w taki sposób ¿e dokonywana jest ekstrakcja t³a dla ci¹gu klatek i zapisane jest ono raz. Nastêpnie wszystkie poruszaj¹ce obiekty zapisane s¹ w ka¿dej klatce jako prostok¹ty otaczaj¹ce obiekt.
/// </summary>
/// <seealso cref="IPacker" />
class DefaultPacker : public IPacker
{
public:
	DefaultPacker(int imageCompression, int backgroundHistory = 300, int differenceThreshold = 20, float newBackgroundMinPixels = 0.2, int minCrumbleArea = 100, int mergeCrumblesIterations = 3);

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
	virtual std::shared_ptr<sequence::IVideo> getVideo() { assert(false); return std::shared_ptr<sequence::IVideo>(); };

private:
	std::shared_ptr<sequence::CompressedVideo> m_CompressedVideo;
	cv::Ptr<cv::BackgroundSubtractor> m_BackgroundSubtractor;
	cv::Mat m_LastBackground;
	size_t m_BackgroundFrame;

	int m_ImageCompression;
	int m_BackgroundHistory;	// 300
	int m_DifferenceThreshold;	// 20
	float m_NewBackgroundMinPixels;		// 0.2
	int m_MinCrumbleArea;		// 100
	int m_MergeCrumblesIterations;	// 3

	void resetBackgroundSubtractor();
};

#endif // DefaultPacker_h__
