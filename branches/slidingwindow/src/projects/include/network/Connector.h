#pragma once
#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QtCore/QObject>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "NetworkMessage.h"

namespace Network
{
	class Connector : public QObject
	{
		Q_OBJECT

	public:
		//! Constructor.
		/*!
		\param _remoteIp that object will connect to.
		\param _remotePort that object will connect to.
		*/
		Connector(const std::string& _remoteIp, const int _remotePort);
		//! Destructor.
		~Connector();

		/*!
		\param _message that is send over network.
		*/
		void send(const std::string& _message);
		/*!
		\return remoteIp that object is connected to.
		*/
		const std::string remoteIp() const { return remoteIp_; }
		/*!
		\return remotePort that object is connected to.
		*/
		const int remotePort() const { return remotePort_; }
		/*!
		\return true if object is connected.
		*/
		const bool isConnected() const { return isConnected_; }

	private:
		//! Fires when connection is established.
		/*!
		\param _error when something went wrong.
		*/
		void handleConnect(const boost::system::error_code& _error);	
		//! Fires when receive message from network.
		/*!
		\param _error when something went wrong.
		*/
		void handleRead(const boost::system::error_code& _error);		

		/*!
		\param _message send over network.
		*/
		void write(NetworkMessage _message);
		//! Fires when write to network was successful.
		/*!
		\param _error when something went wrong.
		*/
		void handleWrite(const boost::system::error_code& _error);

		//! Tries to close socket on both sides nicely if not then brute force.
		void doClose();

		/*!
		\param _isConnected sets internal connection state.
		*/
		void setConnectionState(const bool _isConnected);
		
		//! Service that takes care of all network stuff.
		boost::asio::io_service ioService_;
		//! Tcp socket.
		boost::asio::ip::tcp::socket socket_;
		//! Thread that network service works on.
		boost::shared_ptr<boost::thread> thread_;

		//! Received message from network.
		NetworkMessage readMessage_;
		//! Queue of messages that will be send over network.
		NetworkMessageQueue writeMessagesQueue_;	

		//! Ip that object will connect to.
		const std::string remoteIp_;
		//! Port that object will connect to.
		const int remotePort_;

		//! Internal connection state.
		bool isConnected_;

	signals:
		//! Fires when message has been received.
		/*!
		\param _message that came.
		\param _ip from where message came from.
		*/
		void receivedMessageSignal(const QString& _message, const QString& _ip);
		//! Fires when there is successful connection.
		void connectorConnectionSuccessSignal();
		//! Fires when there is error on connection.
		void connectorConnectionErrorSignal();
	};
}

#endif // CONNECTOR_H
