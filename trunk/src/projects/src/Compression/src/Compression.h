#pragma once
#ifndef Compression_h__
#define Compression_h__

#include <utils/PipeProcess.h>
#include <utils/IAppModule.h>
#include <utils/BlockingQueue.h>

#include <opencv2\core.hpp>

namespace cv
{
	class BackgroundSubtractor;
}

namespace sequence
{
	class IVideo;
	class CompressedVideo;
}

class Compression : public utils::PipeProcess
{
public:
	~Compression();

	static utils::IAppModule* create();
	static void registerParameters(ProgramOptions& programOptions);
	
	virtual bool loadParameters(const ProgramOptions& options) override;

	virtual void reserve() override;
	virtual void process() override;
	virtual void finalize() override;

private:
	Compression();

	static const std::string INPUT_FILE_EXTENSION;
	static const std::string OUTPUT_FILE_EXTENSION;

	int m_FilesInPackage;

	int m_OutFileNr;
	int m_InFileNr;
	int m_FirstFileNr;

	std::unique_ptr<utils::FileLock> m_FileLock;

	std::shared_ptr<sequence::CompressedVideo> m_CompressedVideo;
	cv::Ptr<cv::BackgroundSubtractor> m_BackgroundSubtractor;	
	cv::Mat m_LastBackground;
	size_t m_BackgroundFrame;
	std::shared_ptr<utils::BlockingQueue<cv::Mat>> m_FramesQueue;
	std::mutex m_CoutMutex;

	int m_ImageCompression;

	int m_BackgroundHistory;	// 300
	int m_DifferenceThreshold;	// 20
	float m_NewBackgroundMinPixels;		// 0.2
	int m_MinCrumbleArea;		// 100
	int m_MergeCrumblesIterations;	// 3

	void resetBackgroundSubtractor();

	std::string getOutFileName() const;
	std::string getInFileName() const;

	void decompressFramesThreadFunc(const std::shared_ptr<sequence::IVideo>& video);
	void compressFile(const std::string& fileName);
	void save();
	void cleanup();
};

#endif // Compression_h__