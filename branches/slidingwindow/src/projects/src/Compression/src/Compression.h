#pragma once
#ifndef Compression_h__
#define Compression_h__

#include <utils/PipeProcess.h>
#include <utils/IAppModule.h>
#include <utils/BlockingQueue.h>

#include <opencv2/core.hpp>

#include <boost/timer/timer.hpp>

namespace sequence
{
	class CompressedVideo;
}

namespace sequence
{
	class IVideo;
}

class IPacker;

/// <summary>
/// klasa implementuj¹ca szkielet modu³u. Zawiera funkcjonalnoæ ustawiania parametrów, przetwarzania plików potoku oraz nadzorowanie ca³ego procesu.
/// </summary>
/// <seealso cref="utils::PipeProcess" />
class Compression : public utils::PipeProcess
{
public:
	~Compression();

	/// <summary>
	/// Creates this instance.
	/// </summary>
	/// <returns></returns>
	static utils::IAppModule* create();
	/// <summary>
	/// Registers the parameters.
	/// </summary>
	/// <param name="programOptions">The program options.</param>
	static void registerParameters(ProgramOptions& programOptions);
	
	/// <summary>
	/// Loads the parameters.
	/// </summary>
	/// <param name="options">The options.</param>
	/// <returns></returns>
	virtual bool loadParameters(const ProgramOptions& options) override;

	/// <summary>
	/// Reserves files.
	/// </summary>
	virtual void reserve() override;
	/// <summary>
	/// Processes files.
	/// </summary>
	virtual void process() override;
	/// <summary>
	/// Saves files.
	/// </summary>
	virtual void finalize() override;
	std::shared_ptr<IPacker> glueshift(std::shared_ptr<sequence::IVideo> previousSeq, std::shared_ptr<sequence::IVideo> currentSeq, int shiftValue);

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
	std::string getCopiedOutFileName() const;
	std::string Compression::getFirstWindowOutFileName() const;
	std::string getSecondWindowOutFileName() const;
	std::string getThirdWindowdOutFileName() const;
	std::string getInFileName() const;

	void decompressFramesThreadFunc(const std::shared_ptr<sequence::IVideo>& video);
	void compressFile(const std::string& fileName);
	void save();
	void cleanup();
	//bool divideBoundingRects(std::vector<cv::Rect>& originalContours, std::vector<cv::Rect>& boundingRects, int divideIterations);

	boost::timer::cpu_timer m_TotalTimer;;
};

#endif // Compression_h__
