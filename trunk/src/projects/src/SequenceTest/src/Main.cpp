#include <sequence/IStreamedVideo.h>
#include <sequence/Video.h>

#include <utils/ProgramOptions.h>
#include <utils/BlockingQueue.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>



typedef long long MilisecondType;

MilisecondType getElapsedMs(const boost::timer::cpu_timer& timer)
{
	return timer.elapsed().wall / (MilisecondType)1000000;
}

utils::BlockingQueue<cv::Mat> g_FrameBuffer;
std::string g_Filename;
bool g_Visualize;

template<typename T>
void printOptionsValue(const std::string& name, T value, size_t nameSize = 10)
{
	std::cout << std::setw(nameSize) << name << ": " << std::boolalpha << value << "\n";
}

void decompressThreadFunc()
{

	std::cout << "=======================================================================\n";
	printOptionsValue("file", g_Filename);
	printOptionsValue("visualize", g_Visualize);
	std::cout << "=======================================================================\n\n";

	boost::timer::cpu_timer timer;
	boost::timer::cpu_timer frameTimer;
	timer.start();
	std::shared_ptr<sequence::IStreamedVideo> video = sequence::IStreamedVideo::create(g_Filename);
	timer.stop();
	MilisecondType loadTime = getElapsedMs(timer);

	MilisecondType maxFrameTime = std::numeric_limits<MilisecondType>::min();
	MilisecondType minFrameTime = std::numeric_limits<MilisecondType>::max();
	
	timer.start();
	cv::Mat frame;
	frameTimer.start();
	size_t frameNr = video->getNumFrames();
	while (video->getNextFrame(frame))
	{
		if (g_Visualize)
			g_FrameBuffer.push(std::move(frame));

		MilisecondType frameTime = getElapsedMs(frameTimer);
		frameTimer.start();
		maxFrameTime = std::max(maxFrameTime, frameTime);
		minFrameTime = std::min(minFrameTime, frameTime);
		//++frameNr;
	}
	if (g_Visualize)
		g_FrameBuffer.push(std::move(cv::Mat()));
	timer.stop();
	MilisecondType elapsed = getElapsedMs(timer);
	std::cout << "results\n";
	std::cout << "     load time: " << loadTime << " ms\n";
	std::cout << "        frames: " << frameNr << "\n";
	std::cout << "    total time: " << elapsed << " ms\n";
	std::cout << "       avg fps: " << double(frameNr) * 1000.0 / double(elapsed) << "\n";
	std::cout << "      min time: " << minFrameTime << " ms\n";
	std::cout << "      max time: " << maxFrameTime << " ms\n";
	std::cout << std::endl;
}

int main(int argc, const char** argv)
{
	ProgramOptions options;
	options.add<std::string>("file", "  video filename");
	options.add("visualize", "  show visualization");
	try
	{
		options.parse(argc, argv);

		if (!options.get<std::string>("file", g_Filename))
			throw std::exception("No filename parameter");

		g_Visualize = options.exists("visualize");				

		boost::thread decompressionThread(decompressThreadFunc);
		
		if (g_Visualize)
		{
			std::string WIN_NAME = "Video";
			cv::Mat frame;
			while (g_FrameBuffer.pop(frame) && !frame.empty())
			{
				cv::imshow(WIN_NAME, frame);
				cv::waitKey(1);
			}
		}

		if (decompressionThread.joinable())
			decompressionThread.join();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;	
		options.printUsage();
	}
	catch (...)
	{
		std::cerr << "Unknown error" << std::endl;
		options.printUsage();
	}

	return 0;
}