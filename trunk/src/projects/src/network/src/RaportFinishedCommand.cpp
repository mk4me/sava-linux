#include "RaportFinishedCommand.h"

#include <sstream>
#include <boost/archive/text_oarchive.hpp>

namespace Network
{
	std::string RaportFinishedCommand::encode()
	{
		std::ostringstream oss;
		boost::archive::text_oarchive oa(oss);
		oa << getType();
		oa << *this;
		return oss.str();
	}
}
