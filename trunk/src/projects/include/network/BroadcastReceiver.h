#pragma once
#ifndef BROADCAST_CLIENT_H
#define BROADCAST_CLIENT_H

#include <QtCore/QObject>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "NetworkMessage.h"

namespace Network
{
	class BroadcastReceiver
	{
	public:
		//! Constructor.
		/*!
		\param _receivePort port for receiving udp data.
		*/
		BroadcastReceiver(const int _receivePort);
		//! Destructor.
		~BroadcastReceiver();

	private:
		//! Async wait function for received data.
		void startReceive();
		//! Function that handles received data.
		/*!
		\param _error when something went wrong.
		\param _bytesTransferred how many bytes was received through network.
		*/
		void handleReceive(const boost::system::error_code& _error,
			std::size_t _bytesTransferred);

		//! Closes udp socket.
		void doClose();

		//! Service that takes care of all network stuff.
		boost::asio::io_service ioService_;
		//! Udp socket.
		boost::asio::ip::udp::socket socket_;
		//! Thread that network service works on.
		boost::shared_ptr<boost::thread> thread_;
		//! Endpoint that socket will be connected to.
		boost::asio::ip::udp::endpoint remoteEndpoint_;

		//! Message received over udp.
		NetworkMessage message_;
	};
}

#endif // BROADCAST_CLIENT_H