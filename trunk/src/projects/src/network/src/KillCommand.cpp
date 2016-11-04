#include "KillCommand.h"

#include <sstream>
#include <boost/archive/text_oarchive.hpp>

namespace Network
{
	std::string KillCommand::encode()
	{
		std::ostringstream oss;
		boost::archive::text_oarchive oa(oss);
		auto t = getType();
		oa << t;
		oa << *this;
		return oss.str();
	}
}
