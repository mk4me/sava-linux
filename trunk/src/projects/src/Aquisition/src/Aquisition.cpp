#include "Aquisition.h"

#include <sequence/Video.h>

#include <config/Aquisition.h>

#include <utils/Filesystem.h>
#include <utils/Application.h>
#include <utils/AxisRawReader.h>
#include <utils/MilestoneRawReader.h>
#include <utils/PipeProcessUtils.h>
#include <utils/FileLock.h>

#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>

const std::string Aquisition::OUTPUT_FILE_EXTENSION("vsq");

const std::string Aquisition::MILESTONE_INPUT_TYPE("milestone");
const std::string Aquisition::AXIS_INPUT_TYPE("axis");

Aquisition::Aquisition()
	: m_SeqLength(-1)
	, m_AquisitionRunning(false)
{

}

Aquisition::~Aquisition()
{

}

utils::IAppModule* Aquisition::create()
{
	return new Aquisition();
}

void Aquisition::registerParameters(ProgramOptions& programOptions)
{
	programOptions.add<std::string>("of", "  output folder");
	programOptions.add<int>("sl", "  sequence length (optional)");
	programOptions.add<std::string>("in", "  input type: milestone or axis", AXIS_INPUT_TYPE);
	programOptions.add<std::string>("ip", "  camera IP string e.g. \"1.2.3.4\" (\"http://\" is added automatically)");
	programOptions.add<std::string>("user", "  camera user name");
	programOptions.add<std::string>("pass", "  camera password");
	programOptions.add<std::string>("guid", "  camera guid");
	//programOptions.add<std::string>("cr", "  camera login e.g. \"user:pwd\" (must be supplied)");
	programOptions.add<size_t>("fps", "  desired frame rate (range: 1 to 25) - 0 means maximum rate (optional)");
	programOptions.add<size_t>("c", "JPEG compression rate from 1 - lowest to 99 - highest (optional)");
	programOptions.add<size_t>("fw", "frame width (optional)", utils::camera::AxisRawReader::DEFAULT_WIDTH);
	programOptions.add<size_t>("fh", "frame height (optional)", utils::camera::AxisRawReader::DEFAULT_HEIGHT);
}

bool Aquisition::loadParameters(const ProgramOptions& options)
{
	try
	{
		bool success = true;

		config::Aquisition& config = config::Aquisition::getInstance();
		config.load();

		success &= options.get<std::string>("of", m_OutputFolder);
		//success &= options.get<std::string>("cr", m_Credentials);
		success &= options.get<std::string>("in", m_InputType);
		success &= options.get<std::string>("user", m_User);
		success &= options.get<std::string>("pass", m_Password);
		success &= options.get<std::string>("ip", m_Ip);
		if (m_InputType == MILESTONE_INPUT_TYPE)
			success &= options.get<std::string>("guid", m_CameraGuid);

		if (!options.get<int>("sl", m_SeqLength))
			m_SeqLength = config.getSeqLength();
		if (!options.get<size_t>("fps", m_Fps))
			m_Fps = config.getFps();
		if (!options.get<size_t>("c", m_Compression))
			m_Compression = config.getCompression();
		success &= options.get<size_t>("fw", m_FrameWidth);
		success &= options.get<size_t>("fh", m_FrameHeight);

		std::cout << "================================================================================\n";
		std::cout << "               Camera IP: " << m_Ip << "\n";
		std::cout << "           Output folder: " << m_OutputFolder << "\n";
		std::cout << "                    User: " << m_User << "\n";
		std::cout << "                Password: " << m_Password << "\n";
		std::cout << " File length (in frames): " << m_SeqLength << "\n";
		std::cout << "        Frame rate (fps): " << m_Fps << "\n";
		std::cout << "        JPEG compression: " << m_Compression << "\n";
		std::cout << "             Frame width: " << m_FrameWidth << "\n";
		std::cout << "            Frame height: " << m_FrameHeight << "\n";
		std::cout << "================================================================================\n\n";

		m_OutputFolder = utils::Filesystem::unifyPath(m_OutputFolder);

		return success;
	}
	catch (...)
	{
		std::cerr << "Aquisition::loadParameters(): wrong parameters" << std::endl;
	}

	return false;
}

bool Aquisition::start()
{
	std::string outputFolder = m_OutputFolder;// +m_Ip + "/";
	try
	{
		boost::filesystem::create_directories(boost::filesystem::path(outputFolder));
	}
	catch (...)
	{
		std::cerr << "Aquisition::start(): Can't create directory \"" << outputFolder << "\"" << std::endl;
		return false;
	}
	try
	{
		if (m_InputType == AXIS_INPUT_TYPE)
			m_FrameReader = std::make_shared<utils::camera::AxisRawReader>(m_Ip, m_User + ":" + m_Password, m_Fps, m_Compression, m_FrameWidth, m_FrameHeight);
		else if (m_InputType == MILESTONE_INPUT_TYPE)
		{
			utils::camera::MilestoneCredentials credentials(m_Ip, m_User, m_Password);
			m_FrameReader = std::make_shared<utils::camera::MilestoneRawReader>(credentials, m_CameraGuid, m_Fps, m_Compression, m_FrameWidth, m_FrameHeight);
		}

		m_AquisitionRunning = true;
		boost::thread t(&Aquisition::aquisitionThreadFunc, this);
		m_AquisitionThread.swap(t);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Aquisition::start() error: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Aquisition::start() Unknown error:" << std::endl;
	}

	return true;
}

void Aquisition::stop()
{
	m_AquisitionRunning = false;
	if (m_AquisitionThread.joinable())
		m_AquisitionThread.join();
	std::cout << "Quitting..." << std::endl;
}

void Aquisition::aquisitionThreadFunc()
{
	int fileNr = 0;
	std::set<int> fileNumbers;
	utils::PipeProcessUtils::getFilesIndices(fileNumbers, m_OutputFolder, m_Ip, OUTPUT_FILE_EXTENSION);
	if (!fileNumbers.empty())
	{
		fileNr = *fileNumbers.rbegin();
	}
	boost::timer::cpu_timer timer;
	while (m_AquisitionRunning)
	{
		std::cout << "Acquisition file number " << fileNr << std::endl;

		std::ostringstream oss;
		oss << m_OutputFolder << m_Ip << "." << fileNr++ << "." << OUTPUT_FILE_EXTENSION;

		utils::FileLock fileLock(oss.str() + ".loc");
		if(!fileLock.lock())
			continue;

		sequence::Video video;
		for (int frameNr = 0; frameNr < m_SeqLength; ++frameNr)
		{
			utils::camera::RawMJPGFrame frame;
			m_FrameReader->popRawFrame(frame);
			video.addFrame(frame.m_TimeStamp, std::move(frame.m_RawFrame));
			std::cout << ".";
		}

		video.save(oss.str());
		fileLock.unlock();

		timer.stop();
		auto elapsed = timer.elapsed().wall / 1000000;
		timer.start(); 
		std::cout << "done.\n\ttime: " << elapsed << " ms\n\tfps: " << float(video.getNumFrames()) * 1000.0f / float(elapsed) << "\n\tframes: " << video.getNumFrames() << "\n\tframes left in buffer: " << m_FrameReader->bufferSize() << std::endl;
		
	}
}


