#pragma once

#include "motionanalysis/algorithms/aux_serialization.h" 

namespace motion_analysis 
{

class ParamWrapperBase
{
public:
	/// Sets parameter from integer value.
	virtual void setInUnits( int unitCount ) = 0;

	/// Gets parameter value as integer.
	virtual int getInUnits() const = 0;

	/// Gets parameter as string.
	virtual std::string getStringValue() const = 0;

	/// Gets parameter precision.
	virtual int getPrecision() const = 0;

	/// Gets parameter name.
	virtual std::string getName() const { return name; }

	virtual std::string toString() const { return getName() + " = " + getStringValue(); }

	/// Creates wrapper for parameter with given name.
	ParamWrapperBase( std::string name ) : name( name ) {}

	/// Does nothing.
	virtual ~ParamWrapperBase() {}

protected:
	/// Parameter name.
	std::string name;

	/// Non-parametric constructor for serialization. 
	ParamWrapperBase() {}

private:
	friend class boost::serialization::access;
	// When the class Archive corresponds to an output archive, the
	// & operator is defined similar to <<.  Likewise, when the class Archive
	// is a type of input archive the & operator is defined similar to >>.
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & SERIALIZATION_NVP(name);
	}
};

}BOOST_SERIALIZATION_ASSUME_ABSTRACT(motion_analysis::ParamWrapperBase);