#pragma once
#include <boost/lexical_cast.hpp>

#include "motionanalysis/algorithms/ParamWrapperBase.h"
#include "motionanalysis/algorithms/aux_serialization.h" 

namespace motion_analysis {

/// <summary>
/// Encapsulates clustering parameters. Allows them to be accessed by name. It also stores some additional features.
/// It can be used e.g. for automatic user interface generation.
/// </summary>
template <class T>
class ParamWrapper : public ParamWrapperBase
{
    public:
    /// Sets parameter from integer value.
    virtual void setInUnits( int unitCount )
    {
        *variable = min + unit * unitCount;
    }

    /// Gets parameter value as integer.
    virtual int getInUnits() const { return static_cast<int>(( *variable - min ) / unit); }

	/// Gets parameter value as string.
    virtual std::string getStringValue() const
    {
        return boost::lexical_cast<std::string>(*variable);
    }

    /// Gets parameter precision.
    virtual int getPrecision() const { return precision; };

    /// <summary>
    /// Creates parameter wrapper.
    /// </summary>
    /// <param name=name>Parameter name.</param>
    /// <param name=variable>Reference to variable storing parameter.</param>
    /// <param name=value>Initial parameter value.</param>
    /// <param name=min>Minimum paramter value.</param>
    /// <param name=max>Maximum paramter value.</param>
	ParamWrapper( std::string name, T& variable, T value, T min, T max, int precision = -1 )
    : ParamWrapperBase( name ), variable( &variable ), min( min ), max( max )
    {
        *(this->variable) = value;
        unit = ( precision == -1 ) ? 1 : ( max - min ) / precision;
        this->precision = ( precision == -1 ) ? static_cast<int>(max - min) : precision;
    }

    protected:
    /// Minimum value of parameter.
	T min;

	/// Maximum value of parameter.
    T max;

	/// Pointer to the variable storing current value of parameter.
    T* variable;

	/// Parameter unit.
    T unit;

	/// Parameter precision.
    int precision;

	/// Non-parametric constructor for serialization. 
	ParamWrapper() : ParamWrapperBase(), variable(nullptr) {}

private:
	friend class boost::serialization::access;
	// When the class Archive corresponds to an output archive, the
	// & operator is defined similar to <<.  Likewise, when the class Archive
	// is a type of input archive the & operator is defined similar to >>.
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & SERIALIZATION_BASE_OBJECT_NVP(ParamWrapperBase);
		
		ar & SERIALIZATION_NVP(min);
		ar & SERIALIZATION_NVP(max);
	//	ar & SERIALIZATION_NVP(variable); 
		ar & SERIALIZATION_NVP(precision);
	}

};

template<>
class ParamWrapper<bool> : public ParamWrapperBase
{
public:
	virtual void setInUnits(int unitCount) { *variable = unitCount > 0; }
	virtual int getInUnits() const { return *variable ? 1 : 0; }
	virtual std::string getStringValue() const 
	{
		return boost::lexical_cast<std::string>(*variable);
	}
	virtual int getPrecision() const { return 1; };

	ParamWrapper(std::string name, bool& variable, bool value, bool min, bool max, int precision = -1)
		: ParamWrapperBase(name), variable(&variable)
	{
		*(this->variable) = value;
	}


protected:
	/// Pointer to the variable storing current value of parameter.
	bool* variable;

	ParamWrapper() : ParamWrapperBase(), variable(nullptr) {}

private:
	friend class boost::serialization::access;
	// When the class Archive corresponds to an output archive, the
	// & operator is defined similar to <<.  Likewise, when the class Archive
	// is a type of input archive the & operator is defined similar to >>.
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & SERIALIZATION_BASE_OBJECT_NVP(ParamWrapperBase);

		//	ar & SERIALIZATION_NVP(variable); 
	}
};

}
