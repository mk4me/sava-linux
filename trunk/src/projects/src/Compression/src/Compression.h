#pragma once
#ifndef Compression_h__
#define Compression_h__

#include <utils/PipeProcess.h>
#include <utils/IAppModule.h>
#include <utils/BlockingQueue.h>

#include <opencv2/core.hpp>

namespace sequence
{
	class CompressedVideo;
}

namespace sequence
{
	class IVideo;
}

class IPacker;

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

	
	std::shared_ptr<utils::BlockingQueue<cv::Mat>> m_FramesQueue;
	std::mutex m_CoutMutex;


	std::shared_ptr<IPacker> m_Packer;

	std::string getOutFileName() const;
	std::string getInFileName() const;

	void decompressFramesThreadFunc(const std::shared_ptr<sequence::IVideo>& video);
	void compressFile(const std::string& fileName);
	void save();
	void cleanup();
	//bool divideBoundingRects(std::vector<cv::Rect>& originalContours, std::vector<cv::Rect>& boundingRects, int divideIterations);
};

#endif // Compression_h__