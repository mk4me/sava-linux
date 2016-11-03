#include "BroadcastReceiver.h"

namespace Network
{
	BroadcastReceiver::BroadcastReceiver(const int _receivePort)
		: socket_(ioService_)
	{		
		boost::system::error_code ec;

		socket_.open(boost::asio::ip::udp::v4(), ec);
		if (ec)
		{
			std::cerr << "ERROR: BroadcastReceiver::BroadcastReceiver() on socket_.open => " << ec.message() << std::endl;
			doClose();
			return;
		}

		socket_.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), _receivePort), ec);
		if (ec)
		{
			std::cerr << "ERROR: BroadcastReceiver::BroadcastReceiver() on socket_.bind => " << ec.message() << std::endl;
			doClose();
			return;
		}
		
		//wszystko ok, odbieramy
		startReceive();

		thread_.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &ioService_)));
	}

	BroadcastReceiver::~BroadcastReceiver()
	{
		doClose();
		ioService_.stop();
		thread_->join();
	}

	void BroadcastReceiver::startReceive()
	{
		socket_.async_receive_from(
			boost::asio::buffer(message_.getBuffer()), remoteEndpoint_,
			boost::bind(&BroadcastReceiver::handleReceive, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}


	void BroadcastReceiver::handleReceive(const boost::system::error_code& _error,
		std::size_t _bytesTransferred)
	{
		if (!_error || _error == boost::asio::error::message_size)
		{	
			std::cout << "BroadcastReceiver Received: '" << message_.getBuffer().c_array() 
				<< "' from " << remoteEndpoint_.address().to_string() << std::endl;

			startReceive();
		}		
	}

	void BroadcastReceiver::doClose()
	{
		if (socket_.is_open())
		{
			boost::system::error_code ec;
			socket_.close(ec);
			if (ec)
				std::cerr << "ERROR: BroadcastReceiver::doClose() => " << ec.message() << std::endl;
		}
	}
}