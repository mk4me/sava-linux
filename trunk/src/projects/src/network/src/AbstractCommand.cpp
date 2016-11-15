#include "AbstractCommand.h"
#include <iostream>
#include <sstream>
#include <boost/archive/text_iarchive.hpp>

#include "StartCommand.h"
#include "StopCommand.h"
#include "KillCommand.h"
#include "RaportErrorCommand.h"
#include "RaportFinishedCommand.h"

namespace Network
{
	std::shared_ptr<AbstractCommand> AbstractCommand::decode(const std::string& _message)
	{
		std::shared_ptr<AbstractCommand> command;

		if (_message.empty())
		{
			std::cerr << "ERROR: AbstractCommand::decode() => _message is empty!!!" << std::endl;
			std::cerr << "CHECK: NetworkMessage::messageLength!!!" << std::endl;		
		}

		try
		{
			std::istringstream ss(_message);
			boost::archive::text_iarchive ia(ss);

			Type type;
			ia >> type;
			
			switch (type)
			{
			case Start:
			{
				auto c = std::make_shared<StartCommand>();
				ia >> *c;
				command = c;
				break;
			}
			case Stop:
			{
				auto c = std::make_shared<StopCommand>();
				ia >> *c;
				command = c;
				break;
			}
			case Kill:
			{
				auto c = std::make_shared<KillCommand>();
				ia >> *c;
				command = c;
				break;
			}
			case RaportError:
			{
				auto c = std::make_shared<RaportErrorCommand>();
				ia >> *c;
				command = c;
				break;
			}
			case RaportFinished:
			{
				auto c = std::make_shared<RaportFinishedCommand>();
				ia >> *c;
				command = c;
				break;
			}
			}
		}
		catch (std::exception& e)
		{
			std::cerr << "ERROR: AbstractCommand::decode() => " << e.what() << std::endl;
			std::cerr << "CHECK: _message: " << _message << std::endl;			
		}

		return command;
	}
}