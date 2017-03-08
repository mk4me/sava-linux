#pragma once
#include <map>
#include <boost/serialization/map.hpp>

#include "ParamWrapper.h"

namespace motion_analysis {

class ParamSet
{
public:
	/// Wrappers for parameters allowing them to be accessed by name and given some additional features.
	std::map<std::string, std::shared_ptr<motion_analysis::ParamWrapperBase>> wrappers;

	/// Translates parameters to string.
	std::string toString() 
	{
		std::stringstream ss;
		for (auto w = wrappers.begin(); w != wrappers.end(); ++w) {
			ss << w->second->toString() << std::endl;
		}
		return ss.str();
	}

protected:
	/// <summary>
	/// Adds parameter wrapper.
	/// </summary>
	/// <param name=name>Parameter name.</param>
	/// <param name=variable>Reference to variable storing parameter.</param>
	/// <param name=value>Initial parameter value.</param>
	/// <param name=min>Minimum paramter value.</param>
	/// <param name=max>Maximum paramter value.</param>
	/// <param name=precision>Indicates in how many discreete units parameter interval is divieded.</param>
	template <class T>
	void addParam( std::string name, T& variable, T value, T min, T max, int precision = -1 )
	{
		wrappers[name] = std::shared_ptr<ParamWrapperBase>( new ParamWrapper<T>( name, variable, value, min, max, precision ) );
	}

private:
	friend class boost::serialization::access;
	// When the class Archive corresponds to an output archive, the
	// & operator is defined similar to <<.  Likewise, when the class Archive
	// is a type of input archive the & operator is defined similar to >>.
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & SERIALIZATION_NVP(wrappers);
	} 
};

}