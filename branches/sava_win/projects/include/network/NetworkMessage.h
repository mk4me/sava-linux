#pragma once
#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

#include <string>
#include <boost/array.hpp>
#include <deque>

namespace Network
{
	class NetworkMessage
	{
	public:
		//! Message size.
		enum { messageLength = 512 };

		//! Constructor.
		NetworkMessage()
		{
			//sava-linux.rev
			//buffer_ = { "" };
		}

		//! Constructor.
		/*!
		\param _message that will be held in internal buffer.
		*/
		NetworkMessage(const std::string& _message)
		{
            //sava-linux.rev
			//buffer_ = { "" };

			if (_message.size() > messageLength)
				return;

			for (int i = 0; i < _message.size(); ++i)
				buffer_[i] = _message.c_str()[i];
		}

		/*!
		\return internal buffer.
		*/
		boost::array<char, messageLength>& getBuffer() { return buffer_; }

	private:
		//! Internal buffer.
		boost::array<char, messageLength> buffer_;
	};

	//! Message queue.
	typedef std::deque<NetworkMessage> NetworkMessageQueue;
}

#endif // NETWORK_MESSAGE_H