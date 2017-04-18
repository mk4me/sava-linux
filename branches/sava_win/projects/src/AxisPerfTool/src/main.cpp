#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <QtWidgets/QApplication>
#include <QtGui/QImage>

// CAxisRawReader
#include <utils/AxisRawReader.h>
// Timers and stuff
#include <utils/FPSCounter.h>
// Program options
#include <boost/program_options.hpp>
namespace po = boost::program_options;

//! Global variables
std::string g_CameraIP;		// Axis camera IP e.g. "172.16.100.26"
std::string g_UserName;		// Axis camera username e.g. "root"
std::string g_Password;		// Axis camera password e.g. "pjwstk_2011"
int			g_Compression;	// Axis camera compression e.g. "30" (default - 30)
int			g_FPS;			// Axis camera desired FPS count - default 0 (Unlimited)
bool		g_RawFPSOnly;	// When true, application displays only RAW FPS values

//! Parses program options
bool parseProgramOptions(int inArgc, const char** inArgv)
{
	// Declare the supported options.
	po::options_description desc("USAGE: AxisPerfTool --IP CameraIP --U UserName --P Password [--C CompVal] [--FPS DesiredFPS] [--RawFPS]");
	desc.add_options()
		("help", "produces this help message")
		("IP", po::value<std::string>(&g_CameraIP), "Axis camera IP e.g. 172.16.100.26")
		("U", po::value<std::string>(&g_UserName), "Axis camera username e.g. root")
		("P", po::value<std::string>(&g_Password), "Axis camera password e.g. pjwstk_2011")
		("C", po::value<int>(&g_Compression)->default_value(30), "Axis camera compression e.g. 20 (default - 30)")
		("FPS", po::value<int>(&g_FPS)->default_value(0), "Axis camera FPS rate e.g. 25 (default 0 - unlimited)")
		("RawFPS", po::bool_switch()->default_value(false), "When set, no video will be displayed - perf test only")
		;

	// Parse options
	po::variables_map vm;
	try
	{
		po::store(po::parse_command_line(inArgc, inArgv, desc), vm);
		po::notify(vm);
	}
	catch (...)
	{
		// Error while parsing options
		std::cout << "Type --help to get list of available options." << std::endl;
		return false;
	}

	// Display help 
	if (vm.count("help")) 
	{
		std::cout << desc << std::endl;
		return false;
	}

	// Check all parameters
	if (!vm.count("IP") || !vm.count("U") || !vm.count("P"))
	{
		std::cout << desc << std::endl;
		return false;
	}

	// Set compression flag
	g_Compression = vm["C"].as<int>();

	// Set FPS flag
	g_FPS = vm["FPS"].as<int>();

	// Set Raw FPS flag
	g_RawFPSOnly = vm["RawFPS"].as<bool>();

	// All parsed ok
	return true;
}

//! TEST: Retrieves OpenCV-style matrix
const cv::Mat getOpenCVMatFromBytes(const unsigned char* inData, const int inDataSize)
{
	// Parameter check
	if (!inData || (inDataSize < 0))
		return cv::Mat();

	// Use Qt loader
	QImage qframe;
	qframe.loadFromData(inData, inDataSize, "JPG");

	// Copy raw data to OpenCV matrix and convert it
	cv::Mat pMat(qframe.height(), qframe.width(), CV_8UC4, qframe.bits(), qframe.bytesPerLine());
	cv::Mat retMat;
	cv::cvtColor(pMat, retMat, CV_RGBA2RGB);

	// Return clone, not temporary reference
	return retMat.clone();
}


//! Main entry point
int main(int argc, const char* argv[])
{
	// Parses program options
	if (!parseProgramOptions(argc, argv))
	{
		system("pause");
		return 1;
	}

	// Create reader (can be done using new or as a class member)
	//
	// WARNING: we get stop on popRawFrame() if camera is not available
	// because class will be trying to reconnect until camera is available
	//
	std::cout << "Streaming from " + g_CameraIP << " (" << (g_RawFPSOnly ? "PERFTEST" : "VIDEOMODE") << ") at " 
		<< g_FPS << " FPS / Compression (%): " << g_Compression << std::endl;
	utils::camera::AxisRawReader myAxisReader(g_CameraIP, g_UserName + ":" + g_Password, g_FPS, g_Compression);

	// Register plugins so getOpenCVMatFromBytes() can convert raw bytes to jpg
	QApplication myQtApp(argc, const_cast<char**>(argv));
	QApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");

	// Read data continously, till ESC is pressed
	cv::Mat tmpFrame;
	std::string wndName = "Axis Raw Stream";
	cv::namedWindow(wndName);

	// FPS counter
	utils::FPSCounter FPS;
	float nowFPS;

	// Temporary object for last frame
	utils::camera::RawMJPGFrame lastFrame;
	while (cv::waitKey(1) != 27)
	{
		// Get last frame (moved to the temp - no copying)
		if (!myAxisReader.popRawFrame(lastFrame))
			continue;

		// Not only raw frames
		if (!g_RawFPSOnly)
		{
			// Convert and display
			tmpFrame = getOpenCVMatFromBytes(lastFrame.m_RawFrame.data(), (int)lastFrame.m_RawFrame.size());
			if (tmpFrame.data)
			{
				cv::putText(tmpFrame, "FPS: " + std::to_string(FPS()), cv::Point(30, 60), CV_FONT_NORMAL, 2, CV_RGB(255, 0, 0));
				cv::imshow(wndName, tmpFrame);
			}
		}

		// Only when measuring Raw FPS
		if (g_RawFPSOnly)
		{
			// Display only when changed
			nowFPS = FPS(); // Get to force an update every frame (but we want to avoid spamming console)
			if (FPS.IsUpdated()) 
				std::cout << "[" << g_CameraIP << "] FPS: " << nowFPS << std::endl;
		}

	}

	return 0;

}
