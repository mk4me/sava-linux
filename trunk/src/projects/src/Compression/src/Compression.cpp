#include "Compression.h"

#include <sequence/CompressedVideo.h>
#include <sequence/Video.h>

#include <config/Compression.h>

#include <utils/PipeProcessUtils.h>

#include <opencv2\video.hpp>

#include <boost/timer/timer.hpp>

const std::string Compression::INPUT_FILE_EXTENSION("vsq");
const std::string Compression::OUTPUT_FILE_EXTENSION("cvs");

Compression::Compression()
	: PipeProcess(INPUT_FILE_EXTENSION, 1)
	, m_BackgroundFrame(0)
{

}

Compression::~Compression()
{

}

utils::IAppModule* Compression::create()
{
	return new Compression();
}

void Compression::registerParameters(ProgramOptions& programOptions)
{
	PipeProcess::registerParameters(programOptions);

	programOptions.add<int>("fn", "  number of files in package (optional)");
	programOptions.add<int>("ic", "  image compression rate (optional)");

	// TODO lepsze opisy
	programOptions.add<int>("backgroundHistory", "  background substraction history (optional)");
	programOptions.add<int>("differenceThreshold", "  difference threshold (optional)");
	programOptions.add<float>("backgroundMinPixels", "  Min percent of pixels in difference matrix to reset background substractor (optional)");	
	programOptions.add<int>("minCrumbleArea", "  min crumble area (optional)");
	programOptions.add<int>("mergeCrumblesIterations", "  merge crumbles iterations (optional)");
}

bool Compression::loadParameters(const ProgramOptions& options)
{
	std::cout << "================================================================================\n";
	if (!PipeProcess::loadParameters(options))
		return false;

	config::Compression config;
	config.load();

	if (!options.get<int>("fn", m_FilesInPackage))
		m_FilesInPackage = config.getFilesInPackage();
	if (!options.get<int>("ic", m_ImageCompression))
		m_ImageCompression = config.getImageCompression();
	if (!options.get<int>("backgroundHistory", m_BackgroundHistory))
		m_BackgroundHistory = config.getBackgroundHistory();
	if (!options.get<int>("differenceThreshold", m_DifferenceThreshold))
		m_DifferenceThreshold = config.getDifferenceThreshold();
	if (!options.get<float>("backgroundMinPixels", m_NewBackgroundMinPixels))
		m_NewBackgroundMinPixels = config.getNewBackgroundMinPixels();
	if (!options.get<int>("minCrumbleArea", m_MinCrumbleArea))
		m_MinCrumbleArea = config.getMinCrumbleArea();
	if (!options.get<int>("mergeCrumblesIterations", m_MergeCrumblesIterations))
		m_MergeCrumblesIterations = config.getMergeCrumblesIterations();

	std::cout << "        Files in package: " << m_FilesInPackage << "\n";
	std::cout << " Image commpression rate: " << m_ImageCompression << "\n";
	std::cout << "      Background history: " << m_BackgroundHistory << "\n";
	std::cout << "    Difference threshold: " << m_DifferenceThreshold << "\n";
	std::cout << "   Background min pixels: " << m_NewBackgroundMinPixels << "\n";
	std::cout << "        Min crumble area: " << m_MinCrumbleArea << "\n";
	std::cout << "================================================================================\n\n";

	return true;
}

void Compression::reserve()
{
	std::set<int> fileNumbers;
	utils::PipeProcessUtils::getFilesIndices(fileNumbers, getInputFolder(), getFilePattern(), INPUT_FILE_EXTENSION);

	//for (auto it = fileNumbers.begin(); it != fileNumbers.end(); ++it)
	for (int fileNr : fileNumbers)
	{
		if (fileNr % m_FilesInPackage != 0)
			continue;

		m_OutFileNr = fileNr / m_FilesInPackage;
		m_FileLock = std::make_unique<utils::FileLock>(getOutFileName() + ".loc");
		if (!m_FileLock->lock())
			continue;

		m_FirstFileNr = fileNr;
		m_InFileNr = m_FirstFileNr;

		if (m_ImageCompression != sequence::CompressedVideo::DEFAULT_COMPRESSION)
			m_CompressedVideo = std::make_shared<sequence::CompressedVideo>(m_ImageCompression);
		else
			m_CompressedVideo = std::make_shared<sequence::CompressedVideo>();

		m_BackgroundFrame = 0;
		if (!m_BackgroundSubtractor)
			resetBackgroundSubtractor();

		std::cout << "\n\n\n-----------------------\nPost-processing for acquisition file nr " << m_OutFileNr << " started..." << std::endl;

		setState(PipeProcess::PROCESS);
		return;
	}

	waitForFile();
}

void Compression::process()
{
	std::string fileName = getInFileName();
	if (!boost::filesystem::exists(fileName))
	{
		waitForFile();
		return;
	}

	utils::FileLock lock(fileName + ".loc");
	if (!lock.lock())
	{
		waitForFile();
		return;
	}

	compressFile(fileName);
	m_InFileNr++;
	if (m_InFileNr - m_FirstFileNr >= m_FilesInPackage)
		setState(FINALIZE);
}

void Compression::decompressFramesThreadFunc(const std::shared_ptr<sequence::IVideo>& video)
{
	try
	{
		boost::timer::cpu_timer timer;
		for (size_t i = 0; i < video->getNumFrames(); i++)
		{
			cv::Mat frame = video->getFrameImage(i);
			m_FramesQueue->push(std::move(frame));
		}
		timer.stop();
		auto elapsed = timer.elapsed().wall / 1000000;
		std::lock_guard<std::mutex> lock(m_CoutMutex);
		std::cout << "decompression frames in " << elapsed << " ms" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::lock_guard<std::mutex> lock(m_CoutMutex);
		std::cerr << "Compression::decompressFramesThreadFunc() error: " << e.what() << std::endl;
		m_FramesQueue->release();
	}
	catch (...)
	{
		std::lock_guard<std::mutex> lock(m_CoutMutex);
		std::cerr << "Compression::decompressFramesThreadFunc() Unknown error." << std::endl;
		m_FramesQueue->release();
	}
}

void Compression::compressFile(const std::string& fileName)
{
	m_FramesQueue = std::make_shared<utils::BlockingQueue<cv::Mat>>();
	std::cout << "-Reading file: " << fileName << "..." << std::flush;
	boost::timer::cpu_timer timer;
	std::shared_ptr<sequence::IVideo> seq = sequence::IVideo::create(fileName);
	timer.stop();
	std::cout << "done in " << timer.elapsed().wall / 1000000 << " ms. frames: " << seq->getNumFrames() << std::endl;
		
	std::cout << "-crumble detection..." << std::endl;
	timer.start();
	cv::Mat mask, background, backgroundGray, frameGray, difference;
	boost::thread decompressFramesThread(&Compression::decompressFramesThreadFunc, this, seq);
	
	std::vector<unsigned> mergeHistogram(m_MergeCrumblesIterations);

	for (size_t i = 0; i < seq->getNumFrames(); i++)
	{
		cv::Mat frame;
		if (!m_FramesQueue->pop(frame))
			return;
		std::vector<sequence::CompressedVideo::Crumble> crumbles;
		try
		{
			m_BackgroundSubtractor->apply(frame, mask);
			m_BackgroundSubtractor->getBackgroundImage(background);

			cv::cvtColor(background, backgroundGray, CV_RGB2GRAY);
			cv::cvtColor(frame, frameGray, CV_RGB2GRAY);

			cv::absdiff(frameGray, backgroundGray, difference);
			cv::threshold(difference, difference, m_DifferenceThreshold, 255, cv::THRESH_BINARY);

			if (!m_LastBackground.empty() && cv::countNonZero(difference) > mask.rows * mask.cols * m_NewBackgroundMinPixels)
			{
				if (i > 0)
				{
					m_CompressedVideo->addBackground(m_LastBackground, m_BackgroundFrame);
					m_BackgroundFrame = i;
				}
				resetBackgroundSubtractor();
				m_BackgroundSubtractor->apply(frame, mask);
				m_BackgroundSubtractor->getBackgroundImage(m_LastBackground);
			}
			else
			{
				m_LastBackground = background;

				cv::Mat kernelOpen = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
				cv::Mat kernelClose = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
				cv::morphologyEx(difference, difference, cv::MORPH_ERODE, kernelOpen, cv::Point(-1, -1), 1);
				cv::morphologyEx(difference, difference, cv::MORPH_DILATE, kernelClose, cv::Point(-1, -1), 1);

				std::vector<std::vector<cv::Point>> contours;
				cv::findContours(difference, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

				for (int i = 0; i < m_MergeCrumblesIterations; ++i)
				{
					cv::Mat boundingFrame = cv::Mat::zeros(difference.rows, difference.cols, difference.type());
					for (auto& c : contours)
					{
						cv::Rect br = cv::boundingRect(c);
						cv::rectangle(boundingFrame, br, 255, -1);
					}
					size_t contoursCount = contours.size();
					contours.clear();
					cv::findContours(boundingFrame, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
					if (contours.size() >= contoursCount)
					{
						++mergeHistogram[i];
						break;
					}
				}
				for (auto& c : contours)
				{
					cv::Rect br = cv::boundingRect(c);
					if (br.area() > m_MinCrumbleArea)
					{
						crumbles.push_back(sequence::CompressedVideo::Crumble(br.tl(), cv::Mat(frame, br)));
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			std::lock_guard<std::mutex> lock(m_CoutMutex);
			std::cerr << "Compression::compressFile() error: " << e.what() << std::endl;
		}
		catch (...)
		{
			std::lock_guard<std::mutex> lock(m_CoutMutex);
			std::cerr << "Compression::compressFile() Unknown error" << std::endl;
		}
		m_CompressedVideo->addFrame(seq->getFrameTime(i), crumbles);
	}
	if (decompressFramesThread.joinable())
		decompressFramesThread.join();
	timer.stop();
	auto elapsed = timer.elapsed().wall / 1000000;
	std::lock_guard<std::mutex> lock(m_CoutMutex);
	std::cout << "done.\n";
	std::cout << "\ttime: " << elapsed << " ms\n"; 
	std::cout << "\tfps: " << float(seq->getNumFrames()) * 1000.0f / float(elapsed) << "\n";
	std::cout << "\tframes: " << m_CompressedVideo->getNumFrames() << "\n";
	std::cout << "\tmerge histogram:";
	for (unsigned i : mergeHistogram)
	{
		std::cout << " " << i;
	}
	std::cout << std::endl;
}

void Compression::finalize()
{
	save();
	m_CompressedVideo.reset();
	cleanup();
	m_FileLock.reset();
	setState(PipeProcess::RESERVE_FILE);
}

void Compression::save()
{
	if (!m_CompressedVideo)
		return;
	m_CompressedVideo->addBackground(m_LastBackground, 0);
	m_CompressedVideo->save(getOutFileName());
	std::cout << std::endl << "Post-processed file " << getOutFileName() << " saved." << std::endl;
}

void Compression::cleanup()
{
	auto inputFile = getInputFolder();
	inputFile += getFilePattern();

	for (int i = m_FirstFileNr; i < m_FirstFileNr + m_FilesInPackage; ++i)
	{
		std::stringstream fn;
		fn << inputFile.string() << "." << i << "." << INPUT_FILE_EXTENSION;
		try
		{
			if (boost::filesystem::exists(fn.str()))
				boost::filesystem::remove(fn.str());
		}
		catch (...)
		{
			std::cerr << "Compression::cleanup(): Can't remove file \"" << fn.str() << "\"" << std::endl;
			if (i == m_FirstFileNr)
				return;
		}
	}
}

std::string Compression::getOutFileName() const
{
	std::stringstream ofn;
	FilesystemPath outputFile = getOutputFolder();
	outputFile += getFilePattern();
	ofn << outputFile.string() << "." << m_OutFileNr << "." << OUTPUT_FILE_EXTENSION;
	return ofn.str();
}

std::string Compression::getInFileName() const
{
	std::stringstream fn;
	FilesystemPath inputFile = getInputFolder();
	inputFile += getFilePattern();
	fn << inputFile.string() << "." << m_InFileNr << "." << INPUT_FILE_EXTENSION;
	return fn.str();
}

void Compression::resetBackgroundSubtractor()
{
	m_BackgroundSubtractor = cv::createBackgroundSubtractorMOG2(m_BackgroundHistory);
}
