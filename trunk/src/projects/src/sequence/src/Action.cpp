#include "Action.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>

namespace sequence
{
	Action::Action(const std::string& filename)
	{
		load(filename);
	}

	bool Action::load(const std::string& filename)
	{
		try
		{
			std::ifstream ifs(filename, std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::Action::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Action::save(const std::string& filename) const
	{
		try
		{
			std::ofstream ofs(filename, std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::Action::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

}