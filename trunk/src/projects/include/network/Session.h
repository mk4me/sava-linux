#pragma once
#ifndef SESSION_H
#define SESSION_H

#include "SessionHub.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

namespace Network
{
	class Session : public SessionUser,
		public boost::enable_shared_from_this<Session>
	{
	public:
		//! Constructor.
		/*!
		\param _ioService reference to service that takes care of all network stuff.
		\param _hub that holds all users.
		*/
		Session(boost::asio::io_service& _ioService, SessionHub& _hub);

		/*!
		\return reference to tcp socket.
		*/
		boost::asio::ip::tcp::socket& getSocket();
		/*!
		\param _localIp ip of local endpoint.
		\param _remoteIp ip of remote endpoint.
		*/
		void start(std::string& _localIp, std::string& _remoteIp);
		/*!
		\param _message that will be delivered to session users.
		*/
		void deliver(const NetworkMessage& _message);
		//! Receive message over network.
		/*!
		\param _error when something went wrong.
		*/
		void handleRead(const boost::system::error_code& _error);
		//! Sends message over network.
		/*!
		\param _error when something went wrong.
		*/
		void handleWrite(const boost::system::error_code& _error);

	private:
		//! Tcp socket.
		boost::asio::ip::tcp::socket socket_;
		//! Reference to session hub.
		SessionHub& hub_;

		//! Received message from network.
		NetworkMessage readMessage_;
		//! Queue of messages that will be send over network.
		NetworkMessageQueue writeMessagesQueue_;
	};

	//! Shared pointer of session.
	typedef boost::shared_ptr<Session> SessionPtr;
}


#endif // SESSION_H