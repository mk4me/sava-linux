#include "RaportErrorCommand.h"

#include <sstream>
#include <boost/archive/text_oarchive.hpp>

namespace Network
{
	std::string RaportErrorCommand::encode()
	{
		std::ostringstream oss;
		boost::archive::text_oarchive oa(oss);
		auto t = getType();
		oa << t;
		oa << *this;
		return oss.str();
	}
}
