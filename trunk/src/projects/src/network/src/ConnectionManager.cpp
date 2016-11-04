#include "ConnectionManager.h"
#include <QtCore/QThread>

namespace Network
{
	ConnectionManager::ConnectionManager() : autoReConnectForAll_(true)
	{
		findLocalIp();
	}

	ConnectionManager::ConnectionManager(const std::string& _localIp) : autoReConnectForAll_(true)
	{
		localIp_ = _localIp;
	}

	ConnectionManager::~ConnectionManager()
	{
		for (auto it = connections_.begin(); it != connections_.end(); ++it)
			(*it)->disconnect();
	}

	void ConnectionManager::send(const std::string& _message)
	{
		for (auto it = connections_.begin(); it != connections_.end(); ++it)
			(*it)->send(_message);
	}

	void ConnectionManager::sendCommand(std::shared_ptr<AbstractCommand> _abstractCommand)
	{
		for (auto it = connections_.begin(); it != connections_.end(); ++it)
		{
			if (_abstractCommand->isThisForMe((*it)->remoteIp()))
				(*it)->send(_abstractCommand->encode());
		}
	}

	void ConnectionManager::findLocalIp()
	{
		boost::asio::io_service ioService;
		boost::asio::ip::tcp::resolver resolver(ioService);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
		boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end; // End marker.
		boost::asio::ip::tcp::endpoint ep;		
		
		int interfaceCounter = 0;
		bool bound = false;

		std::cout << "ConnectionsManager will bind to first IPv4 interface." << std::endl;

		while (iter != end)
		{
			ep = *iter++;
			std::cout << "ConnectionsManager found interface " << interfaceCounter++ << ": " << ep.address().to_string();

			if (!bound && ep.address().is_v4())
			{
				localIp_ = ep.address().to_string();
				bound = true;
				std::cout << " => SELECTED!";
			}

			std::cout << std::endl;
		}

		if (interfaceCounter > 1)
			std::cerr << "ConnectionsManager found more than one network interface and may connect to wrong one!!!" << std::endl;		
	}

	ConnectionManager::ConnectorPtr ConnectionManager::findConnector(const std::string& _remoteIp)
	{
		auto it = std::find_if(connections_.begin(), connections_.end(), [&](ConnectorPtr c){return c->remoteIp() == _remoteIp; });
		if (it == connections_.end())
			return ConnectorPtr();
		return *it;
	}

	void ConnectionManager::createConnection(const std::string& _remoteIp, const int _remotePort, const bool _autoReconnect)
	{
		connections_.push_back(std::make_shared<Connector>(_remoteIp, _remotePort));

		QObject::connect(connections_.back().get(), &Connector::receivedMessageSignal, 
			this, &ConnectionManager::handleReceiveMessage, Qt::DirectConnection);
		QObject::connect(connections_.back().get(), &Connector::connectorConnectionSuccessSignal,
			this, &ConnectionManager::handleConnectorConnectionSuccess, Qt::QueuedConnection);

		if (autoReConnectForAll_ && _autoReconnect)
			QObject::connect(connections_.back().get(), SIGNAL(connectorConnectionErrorSignal()),
			this, SLOT(handleConnectorConnectionError()), Qt::QueuedConnection);
	}

	bool ConnectionManager::isConnected(const std::string& _remoteIp)
	{
		ConnectorPtr con = findConnector(_remoteIp);
		if (!con)
			return false;
		return con->isConnected();
	}

	void ConnectionManager::handleReceiveMessage(const QString& _message, const QString& _ip)
	{
		auto receivedCommand = AbstractCommand::decode(_message.toStdString());
		if (receivedCommand == nullptr || !receivedCommand->isThisForMe(localIp_))
			return;

		std::cout << "RECEIVED: " << _message.toStdString() << std::endl;

		switch (receivedCommand->getType())
		{
		case AbstractCommand::RaportError:
		{
			std::shared_ptr<RaportErrorCommand> raport
				= std::dynamic_pointer_cast<RaportErrorCommand>(receivedCommand);
			emit receivedRaportErrorSignal(*raport, _ip);
			break;
		}
		case AbstractCommand::RaportFinished:
		{
			std::shared_ptr<RaportFinishedCommand> raport
				= std::dynamic_pointer_cast<RaportFinishedCommand>(receivedCommand);
			emit receivedRaportFnishedSignal(*raport, _ip);
			break;
		}
		default:
			break;
		}
	}

	void ConnectionManager::handleConnectorConnectionSuccess()
	{
		Connector* connector = dynamic_cast<Connector*>(QObject::sender());
		if (connector == nullptr)
			return;

		emit connectionSuccessSignal(
			QString::fromStdString(connector->remoteIp()), connector->remotePort());
	}

	void ConnectionManager::handleConnectorConnectionError()
	{
		Connector* connector = dynamic_cast<Connector*>(QObject::sender());
		if (connector == nullptr)
			return;

		//musimy zapisac, bo po erase juz obiekt nie istenieje
		std::string remoteIp = connector->remoteIp();
		int remotePort = connector->remotePort();

		for (auto it = connections_.begin(); it != connections_.end();)
		{
			if ((*it).get() == connector)
				it = connections_.erase(it);
			else
				++it;
		}

		QThread::sleep(1);
		createConnection(remoteIp, remotePort);
	}
}