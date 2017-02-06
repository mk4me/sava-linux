#pragma once
#ifndef SESSION_HUB_H
#define SESSION_HUB_H

#include <QtCore/QObject>
#include <set>
#include "SessionUser.h"
#include <boost/asio.hpp>

namespace Network
{
	class SessionHub : public QObject
	{
		Q_OBJECT

	public:
		/*!
		\param _user that joins session hub.
		*/
		void join(SessionUserPtr _user);
		/*!
		\param _user that leaves session hub.
		\param _error when something went wrong.
		*/
		void leave(SessionUserPtr _user, const boost::system::error_code& _error);
		/*!
		\param _message that will be delivered to session users.
		*/
		void deliver(const NetworkMessage& _message);
		//! Fires signal that message has been received.
		/*!
		\param _message that will passed to some slot connected to hubReceivedMessageSignal.
		*/
		void emitMessage(const std::string& _message);

	private:
		//! Session users.
		std::set<SessionUserPtr> sessionUsers_;

	signals:
		//! Fires when message has been received.
		/*!
		\param _message that will passed to some slot connected to this signal.
		*/
		void hubReceivedMessageSignal(const QString& _message);
		//! Fires when connection has been closed.
		void hubConnectionClosedSignal();
	};
}

#endif // SESSION_HUB_H