#pragma once

namespace motion_analysis {

class ClusterBase
{
public:
	/// Cluster size.
	virtual const ::size_t elementsCount() const = 0;

	/// <summary>
	/// Virtual destructor.
	/// </summary>
	virtual ~ClusterBase() {}; 

private:
	friend class boost::serialization::access;
	// When the class Archive corresponds to an output archive, the
	// & operator is defined similar to <<.  Likewise, when the class Archive
	// is a type of input archive the & operator is defined similar to >>.
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		SERIALIZATION_LOG("ClusterBase done!"); 
	}
};

}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(motion_analysis::ClusterBase);