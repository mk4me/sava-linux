#include "Acceptor.h"
#include "NetworkMessage.h"

namespace Network
{
	Acceptor::~Acceptor()
	{
		ioService_.stop();
		thread_->join();
	}

	void Acceptor::start(const std::string& _localIp, const int _localPort)
	{
		log("Acceptor will bind to " + QString(_localIp.c_str()) + ":" + QString::number(_localPort));
		initInterface(boost::asio::ip::tcp::endpoint(
			boost::asio::ip::address::from_string(_localIp), _localPort));
	}

	void Acceptor::start(const int _localPort)
	{
		boost::asio::ip::tcp::resolver resolver(ioService_);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), std::to_string(_localPort));
		boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end; // End marker.
		boost::asio::ip::tcp::endpoint ep;
		
		int interfaceCounter = 0;
		bool bound = false;
		boost::asio::ip::tcp::endpoint ep_bound;

		log("Acceptor will bind to first IPv4 interface.");

		while (iter != end)
		{
			ep = *iter++;
			std::stringstream s;
			s << "Acceptor found interface " << interfaceCounter++ << ": " << ep.address().to_string();
	
			if (!bound && ep.address().is_v4())
			{
				bound = true;
				ep_bound = ep;				
				s << ":" << _localPort << " => SELECTED!";
			}

			log(s.str().c_str());
		}

		if (interfaceCounter > 1)
			log("Acceptor found more than one network interface and may connect to wrong one!!!", true);

		initInterface(ep_bound);
	}

	void Acceptor::initInterface(const boost::asio::ip::tcp::endpoint& _endpoint)
	{
		acceptor_.reset(new boost::asio::ip::tcp::acceptor(ioService_, _endpoint));

		//nasluchuj wiadomosci z huba
		QObject::connect(&hub_, SIGNAL(hubReceivedMessageSignal(const QString&)),
			this, SLOT(handleReceiveMessageFromHub(const QString&)), Qt::DirectConnection);
		QObject::connect(&hub_, SIGNAL(hubConnectionClosedSignal()),
			this, SLOT(handleConnectionClosedFromHub()), Qt::DirectConnection);

		log("Acceptor Started");

		startAccept();
		thread_.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &ioService_)));
	}

	void Acceptor::startAccept()
	{
		SessionPtr newSession(new Session(ioService_, hub_));
		acceptor_->async_accept(newSession->getSocket(),
			boost::bind(&Acceptor::handleAccept, this, newSession,
				boost::asio::placeholders::error));
	}

	void Acceptor::handleAccept(SessionPtr _session, const boost::system::error_code& _error)
	{
		if (!_error)
		{
			int localPort = -1;
			int remotePort = -1;

			_session->start(localIp_, remoteIp_, localPort, remotePort);

			log("New session established with "
				"local endpoint " + QString::fromStdString(localIp_) + ":" + QString::number(localPort) + " and "
				"remote endpoint " + QString::fromStdString(remoteIp_) + ":" + QString::number(remotePort));
		}

		startAccept();
	}

	void Acceptor::log(const QString& _log, bool _isError)
	{
		emit acceptorLogSignal(_log, _isError);

		//zostawiam to na wypadek apki konsolowej
// 		if (_isError)
// 			std::cerr << _log.toStdString() << std::endl;
// 		else
// 			std::cout << _log.toStdString() << std::endl;
	}

	void Acceptor::send(const std::string& _message)
	{
		 hub_.deliver(NetworkMessage(_message));
	}

	void Acceptor::handleReceiveMessageFromHub(const QString& _message)
	{		
		emit receivedMessageSignal(_message);
	}

	void Acceptor::handleConnectionClosedFromHub()
	{
		emit connectionClosedSignal();
	}
}