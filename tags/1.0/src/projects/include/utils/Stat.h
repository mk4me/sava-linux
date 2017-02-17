# pragma once
#include <algorithm>
#include <numeric>
#include <vector>

template<typename T> 
class Stat
{
private:
	std::vector<T> stat;
	
public:
	const std::string name;

	Stat(std::string name = "")
		: name(name)
	{
	}

	void add(T x)
	{
		stat.push_back(x);
	};
	
	void clear()
	{
		stat.clear();
	}

	T median()
	{
		if(stat.empty())
			return 0;

		std::sort(stat.begin(), stat.end());
		return stat[stat.size() / 2];
	}
	
	double mean()
	{
		if(stat.empty())
			return 0.0;

		T r = std::accumulate(stat.begin(), stat.end(), (T) 0);
		return (double) r / stat.size();
	}

	double std()
	{
		return std(mean());
	}
	
	double std(double mean)
	{
		if(stat.empty())
			return 0.0;

		double r = 0.0;

		for(auto p = stat.begin(); p != stat.end(); ++p)
			r += (*p - mean) * (*p - mean);
		
		return std::sqrt(r / stat.size());
	};
};
