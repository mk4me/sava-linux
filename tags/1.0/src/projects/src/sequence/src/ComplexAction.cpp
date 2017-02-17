#include "ComplexAction.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

namespace sequence
{
	ComplexAction::ComplexAction(const std::string& filename)
	{
		load(filename);
	}

	template <class Archive>
	bool _complexActionLoad(const std::string& filename, ComplexAction* self)
	{
		try
		{
			std::ifstream ifs(filename, std::ios::binary | std::ios::in);
			Archive ia(ifs);
			ia >> *self;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "sequence::ComplexAction::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

    bool ComplexAction::load(const std::string& filename)
    {
        return _complexActionLoad<boost::archive::binary_iarchive>(filename, this);
    }
    bool ComplexAction::loadFromText(const std::string& filename)
    {
        return _complexActionLoad<boost::archive::text_iarchive>(filename, this);
    }

    template <class Archive>
    bool _complexActionSave(const std::string& filename, const ComplexAction* self)
    {
        try
        {
            std::ofstream ofs(filename, std::ios::binary | std::ios::out);
            Archive oa(ofs);
            oa << *self;
            return true;
        }
        catch (const std::exception& e)
        {
            std::cerr << "sequence::ComplexAction::save() exception: " << e.what() << std::endl;
            return false;
        }
    }

    bool ComplexAction::save(const std::string& filename) const
    {
        return _complexActionSave<boost::archive::binary_oarchive>(filename, this);
    }

    bool ComplexAction::saveAsText(const std::string& filename) const
    {
        return _complexActionSave<boost::archive::text_oarchive>(filename, this);
    }


    std::string ComplexAction::toString() const
	{
		std::stringstream ss;
		ss << "(ComplexActionId: " << m_ComplexActionId << ")" << std::endl;
		for (std::string cluster : m_Clusters)
			ss << cluster << std::endl;

		return ss.str();
	}

}
