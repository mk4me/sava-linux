#pragma once
#ifndef SESSION_USER_H
#define SESSION_USER_H

#include "NetworkMessage.h"
#include <boost/shared_ptr.hpp>

namespace Network
{
	class SessionUser
	{
	public:
		//! Virtual destructor.
		virtual ~SessionUser() {}
		/*!
		\param _message that will be delivered to session users.
		*/
		virtual void deliver(const NetworkMessage& _message) = 0;
	};

	//! Shared pointer for session user.
	typedef boost::shared_ptr<SessionUser> SessionUserPtr;
}

#endif // SESSION_USER_H