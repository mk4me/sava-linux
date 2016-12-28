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
		//! Constructor.
		ConnectionManager();
		//! Constructor.
		/*!
		\param _localIp where ConnectionManager is instantiated.
		*/
		ConnectionManager(const std::string& _localIp);
		//! Destructor.
		~ConnectionManager();

		//! Send any message through network.
		/*!
		\param _message any kind of string someone would like to send through network.
		*/
		void send(const std::string& _message);
		//! Send command message through network.
		/*!
		\param _abstractCommand send command through network.
		*/
		void sendCommand(std::shared_ptr<AbstractCommand> _abstractCommand);

		//! Create connection
		/*!
		\param _remoteIp of machine to connect to.
		\param _remotePort of machine to connect to.
		\param _autoReconnect if remote machine will disconnect, should we try to reconnect?
		*/
		void createConnection(const std::string& _remoteIp, const int _remotePort, const bool _autoReconnect = true);
		//! Create connection
		/*!
		\param _autoReConnectForAll will force to try to connect to any machine that was disconnected.
		*/
		void setAutoReConnectForAll(const bool _autoReConnectForAll) { autoReConnectForAll_ = _autoReConnectForAll; }

		//! Are we connected to remote machine?
		/*!
		\param _remoteIp of machine to check connection.
		*/
		bool isConnected(const std::string& _remoteIp);

		//! Get ip where ConnectionManager is instantiated.
		/*!
		\return local ip.
		*/
		const std::string& getLocalIp() const { return localIp_; }

	private:	
		//! Shared pointer for Connector.
		typedef std::shared_ptr<Connector> ConnectorPtr;

		//! Finds local ip and assign it to internal localIp_ variable.
		void findLocalIp();
		//! Find Connector object that represents connection with remote machine.
		/*!
		\param _remoteIp of machine that we want to find information.
		\return shared pointer for connection with remote machine.
		*/
		ConnectorPtr findConnector(const std::string& _remoteIp);
		
		//! Connection list to remote machines.
		std::list<ConnectorPtr> connections_;
		//! Local ip where Connection Manager is instantiated.
		std::string localIp_;

		//! Try to connect to all remote machines.
		bool autoReConnectForAll_;

	private slots:
		//! Decide what to do with message that came from certain ip.
		/*!
		\param _message that came from network.
		\param _ip from message was sent.
		*/
		void handleReceiveMessage(const QString& _message, const QString& _ip);
		//! Creates Connector object when connection was established.
		void handleConnectorConnectionSuccess();
		//! Tries to reconnect after error.
		void handleConnectorConnectionError();

	signals:
		//! Signal emitted from handleReceiveMessage when message was error command.
		/*!
		\param _raport that came from network.
		\param _ip of remote machine.
		*/
		void receivedRaportErrorSignal(RaportErrorCommand _raport, QString _ip);
		//! Signal emitted from handleReceiveMessage when message was finish command.
		/*!
		\param _raport that came from network.
		\param _ip of remote machine.
		*/
		void receivedRaportFnishedSignal(RaportFinishedCommand _raport, QString _ip);
		//! Signal emitted from handleConnectorConnectionSuccess.
		/*!
		\param _remoteIp  of remote machine.
		\param _remotePort of remote machine.
		*/
		void connectionSuccessSignal(QString _remoteIp, int _remotePort);
	};
}

#endif // CONNECTION_MANAGER_H
