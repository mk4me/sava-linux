#include "Compression.h"
#include "DefaultPacker.h"
#include "GpuPacker.h"

#include <sequence/Video.h>

#include <config/Compression.h>
#include <config/Camera.h>
#include <config/Diagnostic.h>

#include <utils/PipeProcessUtils.h>
#include <utils/Filesystem.h>
#include <utils/Application.h>

#include <opencv2/video.hpp>

#include <boost/timer/timer.hpp>


const std::string Compression::INPUT_FILE_EXTENSION("vsq");
const std::string Compression::OUTPUT_FILE_EXTENSION("cvs");

Compression::Compression()
	: PipeProcess(INPUT_FILE_EXTENSION, 1)
	
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
	programOptions.add<std::string>("mask", "  regions to omit");
}

bool Compression::loadParameters(const ProgramOptions& options)
{
	std::cout << "================================================================================\n";
	if (!PipeProcess::loadParameters(options))
		return false;

	config::Compression& config = config::Compression::getInstance();
	config.load();

	int imageCompression;
	int backgroundHistory;	// 300
	int differenceThreshold;	// 20
	float newBackgroundMinPixels;		// 0.2
	int minCrumbleArea;		// 100
	int mergeCrumblesIterations;	// 3
	int compressionMethod;

	if (!options.get<int>("fn", m_FilesInPackage))
		m_FilesInPackage = config.getFilesInPackage();
	if (!options.get<int>("ic", imageCompression))
		imageCompression = config.getImageCompression();
	if (!options.get<int>("backgroundHistory", backgroundHistory))
		backgroundHistory = config.getBackgroundHistory();
	if (!options.get<int>("differenceThreshold", differenceThreshold))
		differenceThreshold = config.getDifferenceThreshold();
	if (!options.get<float>("backgroundMinPixels", newBackgroundMinPixels))
		newBackgroundMinPixels = config.getNewBackgroundMinPixels();
	if (!options.get<int>("minCrumbleArea", minCrumbleArea))
		minCrumbleArea = config.getMinCrumbleArea();
	if (!options.get<int>("mergeCrumblesIterations", mergeCrumblesIterations))
		mergeCrumblesIterations = config.getMergeCrumblesIterations();
	if (!options.get<int>("compressionMethod", compressionMethod))
		compressionMethod = config.getCompressionMethod();

	cv::Mat cameraMask;
	std::string mask;
	if (options.get<std::string>("mask", mask))
	{
		config::Camera::getInstance().load();
		cameraMask = config::Camera::getInstance().getMask(mask);
	}

	std::cout << "         Files in package: " << m_FilesInPackage << "\n";
	std::cout << "  Image commpression rate: " << imageCompression << "\n";
	std::cout << "       Background history: " << backgroundHistory << "\n";
	std::cout << "     Difference threshold: " << differenceThreshold << "\n";
	std::cout << "    Background min pixels: " << newBackgroundMinPixels << "\n";
	std::cout << "         Min crumble area: " << minCrumbleArea << "\n";
	std::cout << "Merge crumbles iterations: " << mergeCrumblesIterations << "\n";
	std::cout << "       Compression method: " << compressionMethod << "\n";
	if (compressionMethod == config::Compression::METHOD_GPU)
	{
		std::cout << "                     Mask: " << mask << "\n";
	}
	std::cout << "================================================================================\n\n";
			
	if (compressionMethod == config::Compression::METHOD_GPU)
		m_Packer = std::make_shared<GpuPacker>(imageCompression, backgroundHistory, differenceThreshold, newBackgroundMinPixels, minCrumbleArea, mergeCrumblesIterations, cameraMask);
	else
		m_Packer = std::make_shared<DefaultPacker>(imageCompression, backgroundHistory, differenceThreshold, newBackgroundMinPixels, minCrumbleArea, mergeCrumblesIterations);
	
	config::Diagnostic::getInstance().load();
	if (config::Diagnostic::getInstance().getLogMemoryUsage())
	{
		utils::Application::getInstance()->enableMomoryLogging();
	}

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

		// begin cvs file
		m_TotalTimer.start();

		m_FirstFileNr = fileNr;
		m_InFileNr = m_FirstFileNr;

		m_Packer->createSequence();

		std::cout << "\n\n\n-----------------------\nPost-processing for acquisition file nr " << m_OutFileNr << " started..." << std::endl;		

		setState(PipeProcess::PROCESS);
		return;
	}

	waitForFile();
}

void Compression::process()
{
	std::string fileName = getInFileName();
	if (!utils::Filesystem::exists(fileName))
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
	std::cout << "done in " << timer.elapsed().wall / 1000000 << " ms. frames: " << seq->getNumFrames() << "\n";
	
	std::cout << "-crumble detection...\n";
	timer.start();
	
	boost::thread decompressFramesThread(&Compression::decompressFramesThreadFunc, this, seq);
	
	//std::vector<unsigned> mergeHistogram(m_MergeCrumblesIterations);

	for (size_t i = 0; i < seq->getNumFrames(); i++)
	{
		cv::Mat frame;
		if (!m_FramesQueue->pop(frame))
			return;
		try
		{
			m_Packer->compressFrame(i, frame, seq->getFrameTime(i));
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
	}
	if (decompressFramesThread.joinable())
		decompressFramesThread.join();
	timer.stop();
	auto elapsed = timer.elapsed().wall / 1000000;


	std::lock_guard<std::mutex> lock(m_CoutMutex);
	std::cout << "done.\n";
	std::cout << "Process time: " << elapsed << " ms\n";
	std::cout << "Process fps: " << float(seq->getNumFrames()) * 1000.0f / float(elapsed);

	if (seq->getNumFrames() > 0)
	{
		auto lastFrameTime = seq->getFrameTime(seq->getNumFrames() - 1);
		auto timeDiff = boost::posix_time::microsec_clock::local_time() - lastFrameTime;
		std::cout << "\nCurrent timeout: " << timeDiff;
	}
	std::cout << std::endl;
}

void Compression::finalize()
{
	save();
	cleanup();

	m_TotalTimer.stop();
	std::cout << "Total time: " << m_TotalTimer.elapsed().wall / 1000000 << " ms\n" << std::endl;

	m_FileLock.reset();
	setState(PipeProcess::RESERVE_FILE);
}

void Compression::save()
{
	m_Packer->save(getOutFileName());
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
			if (utils::Filesystem::exists(fn.str()))
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

// bool Compression::divideBoundingRects(std::vector<cv::Rect>& originalContours, std::vector<cv::Rect>& boundingRects, int divideIterations)
// {
// 	if (divideIterations == 0)
// 		return false;
// 
// 	static const int sizeThreshold = 50;
// 	bool erased = false;
// 	for (auto it = boundingRects.begin(); it != boundingRects.end();)
// 	{
// 		cv::Rect& br = *it;
// 		
// 		bool intersect = false;
// 		for (auto& origContour : originalContours)
// 		{
// 			if ((origContour & br).area() > 0)
// 			{
// 				intersect = true;
// 				break;
// 			}
// 		}
// 		if (!intersect)
// 		{
// 			it = boundingRects.erase(it);
// 			erased = true;
// 		}
// 		else
// 		{
// 			int halfWidth = br.width / 2;
// 			int halfHeight = br.height / 2;
// 			std::vector<cv::Rect> rects;
// 			if (halfWidth > sizeThreshold && halfHeight > sizeThreshold)
// 			{
// 				rects.push_back(cv::Rect(br.x, br.y, halfWidth, halfHeight));
// 				rects.push_back(cv::Rect(br.x + halfWidth, br.y, halfWidth, halfHeight));
// 				rects.push_back(cv::Rect(br.x, br.y + halfHeight, halfWidth, halfHeight));
// 				rects.push_back(cv::Rect(br.x + halfWidth, br.y + halfHeight, halfWidth, halfHeight));
// 			}
// 			else if (halfWidth > sizeThreshold)
// 			{
// 				rects.push_back(cv::Rect(br.x, br.y, halfWidth, br.height));
// 				rects.push_back(cv::Rect(br.x + halfWidth, br.y, halfWidth, br.height));
// 			}
// 			else if (halfHeight > sizeThreshold)
// 			{
// 				rects.push_back(cv::Rect(br.x, br.y, br.width, halfHeight));
// 				rects.push_back(cv::Rect(br.x, br.y + halfHeight, br.width, halfHeight));
// 			}
// 
// 			if (!rects.empty() && divideBoundingRects(originalContours, rects, divideIterations - 1))
// 			{
// 				it = boundingRects.erase(it);				
// 				it = boundingRects.insert(it, rects.begin(), rects.end());
// 				it += rects.size();
// 
// 				erased = true;
// 			}
// 		}
// 		if (!erased)
// 			++it;
// 	}
// 	return erased;
// }
