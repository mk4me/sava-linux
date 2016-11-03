#include "RaportErrorCommand.h"

#include <sstream>
#include <boost/archive/text_oarchive.hpp>

namespace Network
{
	std::string RaportErrorCommand::encode()
	{
		std::ostringstream oss;
		boost::archive::text_oarchive oa(oss);
		oa << getType();
		oa << *this;
		return oss.str();
	}
}
