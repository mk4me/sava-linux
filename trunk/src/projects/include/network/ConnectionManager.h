#pragma once
#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include "Connector.h"
#include "RaportErrorCommand.h"
#include "RaportFinishedCommand.h"

namespace Network
{
	class ConnectionManager : public QObject
	{
		Q_OBJECT

	public:
		ConnectionManager();
		ConnectionManager(const std::string& _localIp);
		~ConnectionManager();

		void send(const std::string& _message);
		void sendCommand(std::shared_ptr<AbstractCommand> _abstractCommand);

		void createConnection(const std::string& _remoteIp, const int _remotePort, const bool _autoReconnect = true);
		void setAutoReConnectForAll(const bool _autoReConnectForAll) { autoReConnectForAll_ = _autoReConnectForAll; }

		bool isConnected(const std::string& _remoteIp);

		const std::string& getLocalIp() const { return localIp_; }

	private:	
		typedef std::shared_ptr<Connector> ConnectorPtr;

		void findLocalIp();
		ConnectorPtr findConnector(const std::string& _remoteIp);
		
		std::list<ConnectorPtr> connections_;
		std::string localIp_;

		bool autoReConnectForAll_;

	private slots:
		void handleReceiveMessage(const QString& _message, const QString& _ip);
		void handleConnectorConnectionSuccess();
		void handleConnectorConnectionError();

	signals:
		void receivedRaportErrorSignal(RaportErrorCommand _raport, QString _ip);
		void receivedRaportFnishedSignal(RaportFinishedCommand _raport, QString _ip);
		void connectionSuccessSignal(QString _remoteIp, int _remotePort);
	};
}

#endif // CONNECTION_MANAGER_H
