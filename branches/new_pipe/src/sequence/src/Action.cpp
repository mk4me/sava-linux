#include "Action.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"

namespace sequence
{
	Action::Action(const std::string& filename)
	{
		load(filename);
	}


    template <typename Archive>
	bool _actionLoad(const std::string& filename, Action* self)
	{
		try
		{
			std::ifstream ifs(filename, std::ios::binary | std::ios::in);
			Archive ia(ifs);
			ia >> *self;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::Action::load() exception: " << e.what() << std::endl;
			return false;
		}
        return true;
	}

    bool Action::loadFromText(const std::string& filename)
    {
        return _actionLoad<boost::archive::text_iarchive>(filename, this);
    }
    bool Action::load(const std::string& filename)
    {
        return _actionLoad<boost::archive::binary_iarchive>(filename, this);
    }

	template <class Archive>
	bool _actionSave(const std::string& filename, const Action* action) 
	{
		try {
			std::ofstream ofs(filename, std::ios::binary | std::ios::out);
			Archive oa(ofs);
			oa << *action;
		}
		catch (const std::exception& e) {
			std::cerr << "sequence::Action::save() exception: " << e.what() << std::endl;
			return false;
		}
        return true;
	}

	bool Action::save(const std::string& filename) const
	{
		return _actionSave<boost::archive::binary_oarchive>(filename, this);
	}

	bool Action::saveAsText(const std::string& filename) const
	{
		return _actionSave<boost::archive::text_oarchive >(filename, this);
	}

}