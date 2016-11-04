#pragma once
#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <QtCore/QObject>
#include "Session.h"
#include <boost/thread.hpp>

namespace Network
{
	class Acceptor : public QObject
	{
		Q_OBJECT

	public:			
		//! Destructor.
		~Acceptor();

		/*!
		\param _localIp for network connection.
		\param _localPort for network connection.
		*/
		void start(const std::string& _localIp, const int _localPort);
		//! Tries to auto bind to local network card.
		/*!
		\param _localPort for network connection.
		*/
		void start(const int _localPort);
		
		//! Sends message through network.
		/*!
		\param _message reference for message to be sent.
		*/
		void send(const std::string& _message);	
		/*!
		\return Local ip that object will connect to.
		*/
		const std::string& getLocalIp() const { return localIp_; };
		/*!
		\return Remote ip that object will connect to.
		*/
		const std::string& getRemoteIp() const { return remoteIp_; };

	private:
		//! Fires asynchronous wait for connection.
		void startAccept();
		/*!
		\param _endpoint that get incoming connection.
		*/
		void initInterface(const boost::asio::ip::tcp::endpoint& _endpoint);
		//! Starts new session when connection was successful.
		/*!
		\param _session that handles new connection.
		\param _error when something went wrong.
		*/
		void handleAccept(SessionPtr _session, const boost::system::error_code& _error);		

		//! Emits acceptorLogSignal.
		/*!
		\param _log will use cout.
		\param _isError if true will use cerr.
		*/
		void log(const QString& _log, bool _isError = false);

		//! Service that takes care of all network stuff.
		boost::asio::io_service ioService_;
		//! Accepts network connections.
		boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
		//! Thread that network service works on.
		boost::shared_ptr<boost::thread> thread_;

		//! Hub that holds sessions.
		SessionHub hub_;

		//! Local ip that object will connect to.
		std::string localIp_;
		//! Remote ip that object will connect to.
		std::string remoteIp_;
	
	private slots:
		//! Emits receivedMessageSignal.
		/*!
		\param _message that came from hub.
		*/
		void handleReceiveMessageFromHub(const QString& _message);
		//! Emits connectionClosedSignal.
		void handleConnectionClosedFromHub();

	signals:
		//! Fires when message has been received.
		void receivedMessageSignal(const QString& _message);
		//! Fires when hub closed connection.
		void connectionClosedSignal();
		//! Fires when wants to log.
		void acceptorLogSignal(const QString& _log, bool _isError);
	};
}

#endif // ACCEPTOR_H