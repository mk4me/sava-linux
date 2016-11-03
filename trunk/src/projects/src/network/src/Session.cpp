#include "Session.h"
#include <boost/bind.hpp>
#include <iostream>

namespace Network
{
	Session::Session(boost::asio::io_service& _ioService, SessionHub& _hub)
		: socket_(_ioService), hub_(_hub)
	{
	}

	boost::asio::ip::tcp::socket& Session::getSocket()
	{
		return socket_;
	}

	void Session::start(std::string& _localIp, std::string& _remoteIp)
	{
		_localIp = socket_.local_endpoint().address().to_string();
		_remoteIp = socket_.remote_endpoint().address().to_string();

		hub_.join(shared_from_this());
		boost::asio::async_read(socket_,
			boost::asio::buffer(readMessage_.getBuffer()),
			boost::bind(&Session::handleRead, shared_from_this(),
				boost::asio::placeholders::error));
	}

	void Session::deliver(const NetworkMessage& _message)
	{
		bool writeInProgress = !writeMessagesQueue_.empty();
		writeMessagesQueue_.push_back(_message);
		if (!writeInProgress)
		{
			boost::asio::async_write(socket_,
				boost::asio::buffer(writeMessagesQueue_.front().getBuffer()),
				boost::bind(&Session::handleWrite, shared_from_this(),
					boost::asio::placeholders::error));
		}
	}

	void Session::handleRead(const boost::system::error_code& _error)
	{
		if (!_error)
		{
			std::cout << "Session::handleRead() => " << readMessage_.getBuffer().c_array() << std::endl;
			hub_.emitMessage(std::string(readMessage_.getBuffer().c_array()));

			boost::asio::async_read(socket_,
				boost::asio::buffer(readMessage_.getBuffer()),
				boost::bind(&Session::handleRead, shared_from_this(),
					boost::asio::placeholders::error));
		}
		else
		{
			hub_.leave(shared_from_this(), _error);
		}
	}

	void Session::handleWrite(const boost::system::error_code& _error)
	{
		if (!_error)
		{
			writeMessagesQueue_.pop_front();
			if (!writeMessagesQueue_.empty())
			{
				boost::asio::async_write(socket_,
					boost::asio::buffer(writeMessagesQueue_.front().getBuffer()),
					boost::bind(&Session::handleWrite, shared_from_this(),
						boost::asio::placeholders::error));
			}
		}
		else
		{
			hub_.leave(shared_from_this(), _error);
		}
	}
}