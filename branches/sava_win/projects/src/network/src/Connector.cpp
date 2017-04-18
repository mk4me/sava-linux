#include "Connector.h"
#include <boost/bind.hpp>
#include <QtCore/QThread>

namespace Network
{
	Connector::Connector(const std::string& _remoteIp, const int _remotePort)
		: socket_(ioService_), remoteIp_(_remoteIp), remotePort_(_remotePort), isConnected_(false)
	{
		//ustaw polaczenie
		boost::asio::ip::tcp::resolver::query query(_remoteIp, std::to_string(_remotePort));

		boost::asio::ip::tcp::resolver resolver(ioService_);
		boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
		
		std::cout << "Connector started and trying to connect to " << remoteIp_ << ":" << remotePort_ << std::endl;

		boost::asio::async_connect(socket_, it,
			boost::bind(&Connector::handleConnect, this, boost::asio::placeholders::error));
		
		thread_.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &ioService_)));		
	}

	Connector::~Connector()
	{
		//musimy poczekac na wyslanie wszystkich wiadomosci z bufora
		while (writeMessagesQueue_.size() > 0)
		{
			std::cerr << "Connector::~Connector() = > writeMessagesQueue_ is not empty" << std::endl;
			QThread::sleep(1);
		}

		doClose();
		ioService_.stop();
		thread_->join();
	}

	void Connector::send(const std::string& _message)
	{
		ioService_.post(boost::bind(&Connector::write, this, NetworkMessage(_message)));
	}
	
	void Connector::handleConnect(const boost::system::error_code& _error)
	{
		if (!_error)
		{
			std::cout << "Connection established from local IP: " << socket_.local_endpoint().address().to_string()
				<< " to remote IP: " << socket_.remote_endpoint().address().to_string() << std::endl;

			setConnectionState(true);

			boost::asio::async_read(socket_,
				boost::asio::buffer(readMessage_.getBuffer()),
				boost::bind(&Connector::handleRead, this,
					boost::asio::placeholders::error));
		}
		else
		{
			std::cerr << "ERROR: Connector::handleConnect() => " << _error.message() << std::endl;
			setConnectionState(false);
		}
	}

	void Connector::handleRead(const boost::system::error_code& _error)
	{
		if (!_error)
		{
			std::cout << "Connector::handleRead() => " << readMessage_.getBuffer().c_array() << std::endl;
			emit receivedMessageSignal(QString(readMessage_.getBuffer().c_array()), remoteIp_.c_str());

			boost::asio::async_read(socket_,
				boost::asio::buffer(readMessage_.getBuffer()),
				boost::bind(&Connector::handleRead, this,
					boost::asio::placeholders::error));
		}
		else
		{
			std::cerr << "ERROR: Connector::handleRead() => " << _error.message() << std::endl;
			ioService_.post(boost::bind(&Connector::doClose, this));
			setConnectionState(false);
		}
	}

	void Connector::write(NetworkMessage _message)
	{
		bool writeInProgress = !writeMessagesQueue_.empty();
		writeMessagesQueue_.push_back(_message);		
		if (!writeInProgress)
		{
			boost::asio::async_write(socket_,
				boost::asio::buffer(writeMessagesQueue_.front().getBuffer()),
				boost::bind(&Connector::handleWrite, this,
					boost::asio::placeholders::error));
		}
	}

	void Connector::handleWrite(const boost::system::error_code& _error)
	{
		if (!_error)
		{
			writeMessagesQueue_.pop_front();
			if (!writeMessagesQueue_.empty())
			{
				boost::asio::async_write(socket_,
					boost::asio::buffer(writeMessagesQueue_.front().getBuffer()),
					boost::bind(&Connector::handleWrite, this,
						boost::asio::placeholders::error));
			}
		}
		else
		{
			std::cerr << "ERROR: Connector::handleWrite() => " << _error.message() << std::endl;
			ioService_.post(boost::bind(&Connector::doClose, this));
			setConnectionState(false);
		}
	}

	void Connector::doClose()
	{
		if (socket_.is_open())
		{
			boost::system::error_code ec;
			
			socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
			if (ec)
				std::cerr << "ERROR: Connector::doClose() on shutdown() => " << ec.message() << std::endl;
			
			socket_.close(ec);
			if (ec)
				std::cerr << "ERROR: Connector::doClose() on close() => " << ec.message() << std::endl;			
		}		
	}

	void Connector::setConnectionState(const bool _isConnected)
	{
		isConnected_ = _isConnected;
		if (_isConnected)
			emit connectorConnectionSuccessSignal();
		else
			emit connectorConnectionErrorSignal();
	}
}