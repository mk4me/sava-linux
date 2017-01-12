#include <sequence/IStreamedVideo.h>

#include <utils/ProgramOptions.h>
#include <utils/GpuJpegUtils.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <memory>
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>


typedef std::shared_ptr<sequence::IStreamedVideo> VideoPtr;
typedef std::list<std::shared_ptr<sequence::IStreamedVideo>> VideoList;

VideoList g_VideoList;
VideoList::iterator g_CurrentVideo;

utils::GpuJpegEncoder g_Encoder;

std::string g_InputFolder;
std::string g_AddressIp;
std::string g_ServerPort;

boost::asio::io_service g_IoService;

using boost::asio::ip::tcp;

class TcpConnection
	: public std::enable_shared_from_this<TcpConnection>
{
public:
	typedef std::shared_ptr<TcpConnection> pointer;

	static pointer create(boost::asio::io_service& io_service)
	{
		return pointer(new TcpConnection(io_service));
	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		std::cout << "[" << boost::posix_time::microsec_clock::local_time() << "] new connection (" << socket_.remote_endpoint().address().to_string() << ")" << std::endl;
		std::string message = "HTTP/1.0 200 OK\r\n"
			"Cache-Control: no-cache\r\n"
			"Pragma: no-cache\r\n"
			"Expires: Thu, 01 Dec 1994 16 : 00 : 00 GMT\r\n"
			"Connection: close\r\n"
			"Content-Type: multipart/x-mixed-replace; boundary=myboundary\r\n"
			"\r\n";
		buffer_.assign(message.begin(), message.end());

		boost::asio::async_write(socket_, boost::asio::buffer(buffer_),
			boost::bind(&TcpConnection::handle_write, shared_from_this(),
			boost::asio::placeholders::error));
	}

private:
	TcpConnection(boost::asio::io_service& io_service)
		: socket_(io_service)
		, timer_(io_service)
	{
	}

	void do_write(const boost::system::error_code& error)
	{
		timer_.expires_from_now(boost::posix_time::milliseconds(1000 / 25));

		if (error.value() == 10054)
		{
			std::cout << "[" << boost::posix_time::microsec_clock::local_time() << "] connection closed (" << socket_.remote_endpoint().address().to_string() << ")" << std::endl;
			return;
		}
		if (error)
		{
			std::cerr << "TcpConnection::do_write() error: " << error << std::endl;
			return;
		}

		if (g_CurrentVideo == g_VideoList.end())
		{
			std::cerr << "TcpConnection::do_write() error: empty video list." << std::endl;
			return;
		}

		VideoPtr video = *g_CurrentVideo;

		cv::Mat frame;
		if (!video->getNextFrame(frame))
		{
			++g_CurrentVideo;
			if (g_CurrentVideo == g_VideoList.end())
			{
				g_CurrentVideo = g_VideoList.begin();
				if (g_CurrentVideo == g_VideoList.end())
				{
					std::cerr << "TcpConnection::do_write() error: empty video list." << std::endl;
					return;
				}
			}

			video = *g_CurrentVideo;
			video->setPosition(0);

			if (!video->getNextFrame(frame))
			{
				std::cerr << "TcpConnection::do_write() error: video is empty." << std::endl;
				return;
			}
		}

		std::vector<uchar> image;
		//cv::imencode(".jpg", frame, image);
		cv::cvtColor(frame, frame, CV_BGR2RGB);

		g_Encoder.encode(frame, image);

		buffer_.clear();
		std::string message = "--myboundary\r\n";
		message += "Content-Type: image/jpeg\r\n";
		message += "Content-Length: " + std::to_string(image.size()) + "\r\n";
		message += "\r\n";
		

		buffer_.assign(message.begin(), message.end());
		buffer_.insert(buffer_.end(), image.begin(), image.end());
		buffer_.push_back('\r');
		buffer_.push_back('\n');
		//message_ += (const char*)image.data();
		
		boost::asio::async_write(socket_, boost::asio::buffer(buffer_),
			boost::bind(&TcpConnection::handle_write, shared_from_this(),
			boost::asio::placeholders::error));
	}

	void handle_write(const boost::system::error_code& error)
	{
		if (error.value() == 10054)
		{
			std::cout << "[" << boost::posix_time::microsec_clock::local_time() << "] connection closed (" << socket_.remote_endpoint().address().to_string() << ")" << std::endl;
			return;
		}
		if (error)
		{
			std::cerr << "TcpConnection::handle_write() error: " << error << std::endl;
			return;
		}

		if (timer_.expires_from_now() > boost::posix_time::milliseconds(1))
		{
			timer_.async_wait(
				boost::bind(&TcpConnection::do_write, shared_from_this(),
				boost::asio::placeholders::error));
		}
		else
		{
			do_write(error);
		}		
	}

	tcp::socket socket_;
	std::string message_;

	std::vector<uchar> buffer_;

	boost::asio::deadline_timer timer_;
};

class TcpServer
{
public:
	TcpServer(boost::asio::io_service& io_service, const std::string& address, const std::string& port = "80")
		: acceptor_(io_service)
		, ioService_(io_service)
	{
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::endpoint endpoint;

		endpoint = findEndpoint(address, port);
		acceptor_.open(endpoint.protocol());
		acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor_.bind(endpoint);
		acceptor_.listen();

		std::cout << "Acceptor bind to " << endpoint.address().to_string() << ":" << port << std::endl;

		start_accept();
	}

private:
	boost::asio::ip::tcp::endpoint findEndpoint(const std::string& address, const std::string& port)
	{
		boost::asio::ip::tcp::resolver resolver(ioService_);
		boost::asio::ip::tcp::resolver::query query(address.empty() ? boost::asio::ip::host_name() : address, port);
		boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end; // End marker.
		boost::asio::ip::tcp::endpoint ep;

		int interfaceCounter = 0;
		bool bound = false;
		boost::asio::ip::tcp::endpoint ep_bound;

		//std::cout << "Acceptor will bind to first IPv4 interface.";

		while (iter != end)
		{
			ep = *iter++;
			//std::cout << "Acceptor found interface " << interfaceCounter++ << ": " << ep.address().to_string();

			if (!bound && ep.address().is_v4())
			{
				bound = true;
				ep_bound = ep;
				//std::cout << ":" << port << " => SELECTED!";
			}			

			//std::cout << std::endl;
		}

		//if (interfaceCounter > 1)
		//	std::cout << "Acceptor found more than one network interface and may connect to wrong one!!!" << std::endl;
		
		return ep_bound;
	}
	void start_accept()
	{
		TcpConnection::pointer new_connection =
			TcpConnection::create(acceptor_.get_io_service());

		acceptor_.async_accept(new_connection->socket(),
			boost::bind(&TcpServer::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
	}

	void handle_accept(TcpConnection::pointer new_connection,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			new_connection->start();
		}

		start_accept();
	}

	tcp::acceptor acceptor_;
	boost::asio::io_service& ioService_;
};

void parseOptions(int argc, char** argv)
{
	ProgramOptions options;
	try
	{
		options.add<std::string>("if", "  input folder");
		options.add<std::string>("ip", "  address ip to bind");
		options.add<std::string>("port", "  port to bind", "80");

		options.parse(argc, const_cast<const char**>(argv));

		if (!options.get<std::string>("if", g_InputFolder))
			throw std::runtime_error("Option --if is missing.");

		options.get<std::string>("ip", g_AddressIp);
		options.get<std::string>("port", g_ServerPort);
	}
	catch (const std::exception& e)
	{
		options.printUsage();
		throw;
	}
}

void loadFiles()
{
	std::cout << "Input folder: " << g_InputFolder << std::endl;
	std::vector<std::pair<boost::filesystem::path, int>> names;
	for (auto it = boost::filesystem::directory_iterator(g_InputFolder); it != boost::filesystem::directory_iterator(); ++it)
	{
		auto path = it->path();

		std::string ext = path.extension().string();
		if (ext != ".vsq" && ext != ".cvs" && ext != ".avi" && ext != ".mp4")
			continue;

		std::string idStr = path.stem().string();
		size_t pos = idStr.find_last_of('.');
		if (pos != std::string::npos)
		{
			idStr = idStr.substr(pos + 1);

			names.push_back(std::make_pair(path, std::stoi(idStr)));
		}
	}

	std::sort(names.begin(), names.end(), [](const std::pair<boost::filesystem::path, int>& p0, const std::pair<boost::filesystem::path, int>& p1)
	{
		return p0.second < p1.second;
	});

	size_t loaded = 0;
	size_t progress = 0;
	std::cout << "Loading..." << std::endl;
	for (auto& e : names)
	{
		auto video = sequence::IStreamedVideo::create(e.first.string());
		++loaded;
		size_t realProgress = (loaded * 100) / names.size();
		if (realProgress >= progress + 10)
		{
			progress = realProgress;
			std::cout << progress << "%" << std::endl;
		}
		if (video && video->getNumFrames() > 0)
		{
			g_VideoList.push_back(video);
		}
	}
	g_CurrentVideo = g_VideoList.begin();

	std::cout << g_VideoList.size() << " videos loaded." << std::endl;
}

#ifdef WIN32
#include <Windows.h>
static BOOL exitHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		g_IoService.stop();
		return TRUE;

	case CTRL_C_EVENT:
		g_IoService.stop();
		return TRUE;

	default:
		return FALSE;
	}
}
#else
#include <signal.h>
static void exitHandler(int sig)
{
	g_IoService.stop();
}
#endif // WIN32




void registerExitFunction()
{
#ifdef WIN32
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)exitHandler, TRUE))
		std::cerr << "Cant register exit function!" << std::endl;
#else
	signal(SIGINT, exitHandler);
#endif // WIN32
}

int main(int argc, char** argv)
{
	try
	{
		registerExitFunction();
		parseOptions(argc, argv);
		loadFiles();

		TcpServer server(g_IoService, g_AddressIp/*"172.16.100.89"*/, g_ServerPort);
		g_IoService.run();		
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
	std::cout << "Quitting..." << std::endl;
	return 0;
}