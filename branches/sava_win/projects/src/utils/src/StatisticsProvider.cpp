#include "StatisticsProvider.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

std::string StatisticsProvider::printStats()
{
	std::ostringstream oss;

	for (auto s = statistics.begin(); s != statistics.end(); s++)
	{
		oss << s->first << ": " << s->second << std::endl;
	}

	return oss.str();
}

void StatisticsProvider::saveStats(std::string file)
{
	boost::property_tree::ptree pt;
	
	for (auto s = statistics.begin(); s != statistics.end(); s++)
	{
		pt.put(s->first, s->second);
	}

	boost::property_tree::ini_parser::write_ini(file, pt);
}

std::string StatisticsProvider::printNames(std::string separator)
{
	std::ostringstream oss;
	
	for (auto s = statistics.begin(); s != statistics.end(); s++)
	{
		oss << s->first << separator;
	}

	return oss.str();
}

std::string StatisticsProvider::printValues(std::string separator)
{
	std::ostringstream oss;

	for (auto s = statistics.begin(); s != statistics.end(); s++)
	{
		oss << s->second << separator;
	}

	return oss.str();
}
