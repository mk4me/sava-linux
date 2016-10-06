#pragma once
#include <list>
#include <opencv/cv.h>

#include "motionanalysis/algorithms/aux_serialization.h"

namespace motion_analysis {

	/// <summary>
	/// Abstract base class for all collections of points. It is a base class for both PointCollection and Segment classes.
	/// </summary>
	template <typename T>
	class PointCollectionBase
	{
	public:
		typedef T point_type;
		typedef std::vector<T> collection_type;

		typedef typename collection_type::iterator					iterator;
		typedef typename collection_type::const_iterator			const_iterator;
		typedef typename collection_type::reverse_iterator			reverse_iterator;
		typedef typename collection_type::const_reverse_iterator	const_reverse_iterator;

		/// Cluster size.
		virtual const ::size_t pointsCount() const = 0;

		/// Empty default constructor.
		PointCollectionBase() {}

		/// Virtual destructor.
		virtual ~PointCollectionBase() {} 

	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			SERIALIZATION_LOG("PointCollectionBase done!"); 
		}
	};
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(motion_analysis::PointCollectionBase );
