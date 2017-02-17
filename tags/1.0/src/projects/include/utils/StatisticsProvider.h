#pragma once
#include <string>
#include <map>
#include <sstream>

#define GET_STATS

#ifdef GET_STATS
#define STATS_WRITE(key,value) statistics[key] = static_cast<StatisticsProvider::ValueType>(value)
#define STATS_ADD(key,value) statistics[key] += static_cast<StatisticsProvider::ValueType>(value)
#else
	#define STATS_WRITE(key,value) 
	#define STATS_ADD(key,value) 
#endif

class StatisticsProvider
{
public:
	typedef double ValueType;
	std::map<std::string, ValueType> statistics;

	virtual ~StatisticsProvider() {}

	void clearStats() { statistics.clear(); }

	std::string printStats();

	std::string printNames(std::string separator);

	std::string printValues(std::string separator);

	void saveStats(std::string file);

private:
/*	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{	
		SERIAL_LOG("StatisticsProvider done!");
	}*/
};