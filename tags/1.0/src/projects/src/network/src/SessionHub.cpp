#include "SessionHub.h"
#include <boost/bind.hpp>
#include <iostream>

namespace Network
{
	void SessionHub::join(SessionUserPtr _user)
	{
		sessionUsers_.insert(_user);
	}

	void SessionHub::leave(SessionUserPtr _user, const boost::system::error_code& _error)
	{
		//jezeli rozlaczenie nastapilo 'w miare' normlanie,
		//czyli wywolanie zniszczenie obiektu Connector
		//if (_error == boost::asio::error::eof) //ten boost error nie dziala dobrze wiec zostal zakomentowany aby zakladaki nie zostawaly
		{
			emit hubConnectionClosedSignal();
		}

		sessionUsers_.erase(_user);
	}

	void SessionHub::deliver(const NetworkMessage& _message)
	{
		std::for_each(sessionUsers_.begin(), sessionUsers_.end(),
			boost::bind(&SessionUser::deliver, _1, boost::ref(_message)));
	}

	void SessionHub::emitMessage(const std::string& _message)
	{
		emit hubReceivedMessageSignal(QString::fromStdString(_message));
	}
}