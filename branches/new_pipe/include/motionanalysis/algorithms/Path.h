#pragma once
#include "PointCollection.h"
#include "PathClusteringParams.h"


#include <list>
#include <limits>
#include <opencv/cv.h>

namespace motion_analysis {
    /// <summary>
    /// Represents a path.
    /// </summary>
    class Path : public PointCollection
    {
    public:
        /// Displacement from path to its associated cluster, if one exists.
        cv::Point_<PathPositionType> c;

		/// Path identifier.
		uint64_t id;

		/// Indicates removed path.
		bool removed;

        /// <summary>
        /// Initializes some members.
        /// </summary>
        Path( uint64_t id, int category ) : PointCollection( category ), id(id), c( 0,0 ) {}

        /// <summary>
        /// Appends new point to the path and recalculates tangent coordinates.
        /// </summary>
        void append( PathPositionType x, PathPositionType y, PathTimeType t, float alpha );

		/// Calculates memory usage.
		const ::size_t computeMemoryUsage();

		const ::size_t calculateHash() const;

	protected:
			
		Path() : PointCollection(), id(0xffffffffffffffff), c(0, 0), removed(false) {};

	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			SERIALIZATION_LOG("Path begins!"); 
			
			ar & SERIALIZATION_BASE_OBJECT_NVP(PointCollection);
			ar & SERIALIZATION_NVP(c);
			ar & SERIALIZATION_NVP(id);
			//ar & SERIALIZATION_NVP(removed);

			SERIALIZATION_LOG("Path ends!");
		}
    };

}

BOOST_CLASS_EXPORT_KEY(motion_analysis::Path);
BOOST_CLASS_TRACKING(motion_analysis::Path, boost::serialization::track_selectively);