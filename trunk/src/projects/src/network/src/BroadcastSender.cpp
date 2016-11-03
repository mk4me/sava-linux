#include "BroadcastSender.h"

namespace Network
{
	BroadcastSender::BroadcastSender(const int _connectionPort, const int _sendPort, const std::string& _message)
		: socket_(ioService_), endpoint_(boost::asio::ip::address_v4::broadcast(), _sendPort), threadIsWorking_(false), 
		message_(_message)
	{			
		boost::system::error_code ec;

		socket_.open(boost::asio::ip::udp::v4(), ec);
		if (ec)
		{
			std::cerr << "ERROR: BroadcastSender::BroadcastSender() on socket_.open => " << ec.message() << std::endl;
			doClose();
			return;
		}

		socket_.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), _connectionPort), ec);
		if (ec)
		{
			std::cerr << "ERROR: BroadcastSender::BroadcastSender() on socket_.bind => " << ec.message() << std::endl;
			doClose();
			return;
		}

		socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
		socket_.set_option(boost::asio::socket_base::broadcast(true));

		threadIsWorking_ = true;
		thread_.reset(new boost::thread(boost::bind(&BroadcastSender::threadFunction, this)));
	}

	BroadcastSender::~BroadcastSender()
	{
		threadIsWorking_ = false;
		doClose();
		thread_->join();
	}

	void BroadcastSender::threadFunction()
	{
		while (threadIsWorking_)
		{			 
			socket_.send_to(boost::asio::buffer(message_.getBuffer()), endpoint_);
			std::cout << "BroadcastSender Sent: '" << message_.getBuffer().c_array() << "'" << std::endl;			

			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
	}

	void BroadcastSender::doClose()
	{
		if (socket_.is_open())
		{
			boost::system::error_code ec;
			socket_.close(ec);
			if (ec)
				std::cerr << "ERROR: BroadcastSender::doClose() => " << ec.message() << std::endl;
		}
	}
}