#pragma once
#ifndef BROADCAST_SENDER_H
#define BROADCAST_SENDER_H

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "NetworkMessage.h"

namespace Network
{
	class BroadcastSender
	{
	public:
		//! Constructor.
		/*!
		\param _connectionPort port for udp connection.
		\param _sendPort port for sending udp data.
		\param _message that will be send over udp.
		*/
		BroadcastSender(const int _connectionPort, const int _sendPort, const std::string& _message = "I am Sender");
		//! Destructor.
		~BroadcastSender();

	private:
		//! Sends message from constructor every second.
		void threadFunction();
		//! Closes udp socket.
		void doClose();

		//! Service that takes care of all network stuff.
		boost::asio::io_service ioService_;
		//! Udp socket.
		boost::asio::ip::udp::socket socket_;
		//! Endpoint that socket will be connected to.
		boost::asio::ip::udp::endpoint endpoint_;
		//! Thread that network service works on.
		boost::shared_ptr<boost::thread> thread_;

		//! Should thread still work?
		bool threadIsWorking_;
		//! Message send over udp.
		NetworkMessage message_;
	};
}

#endif // BROADCAST_SENDER_H