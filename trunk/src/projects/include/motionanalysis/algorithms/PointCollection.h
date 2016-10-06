#pragma once
#include <list>
#include <opencv/cv.h>
#include <boost/serialization/list.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "PointCollectionBase.h"
#include "Point.h"


namespace motion_analysis {

    /// <summary>
    /// Represents a collection of points. It is a base class for both Path and Cluster classes.
    /// </summary>
    class PointCollection : public PointCollectionBase<Point>
    {
	public:
		/// User-defined collection category.
        int category;

        /// Collection of points.
		collection_type points;
	
		inline iterator begin() { return points.begin(); }
		inline iterator end() { return points.end(); }
		inline reverse_iterator rbegin() { return points.rbegin(); }
		inline reverse_iterator rend() { return points.rend(); }
		inline const_iterator begin() const { return points.begin(); }
		inline const_iterator end() const { return points.end(); }
		inline const_reverse_iterator rbegin() const { return points.rbegin(); }
		inline const_reverse_iterator rend() const { return points.rend(); }

		/// Gets number of points.
		virtual const ::size_t pointsCount() const { return points.size(); }

		/// Initializes some members
		PointCollection() : PointCollectionBase(), category(0) {}

        /// <summary>
        /// Initializes some members.
        /// </summary>
        PointCollection( int category ) : category( category ) {}

        /// <summary>
        /// Check correctness of point collection (for debugging purposes).
        /// </summary>
        /// <returns>Flag indicating whether collection is ok.</returns>
        bool checkCorrectness();

		/// <summary>
		/// Recalculates tangential coordinates of path.
		/// </summary>
		/// <param name=alpha>Alpha parameter.</param>
		void forwardBackwardFilter( float alpha, bool doForward = true );

		/// <summary>
		/// Clears tangential data.
		/// </summary>
		void clearTangentialData();

	
		virtual Point getMean() const 
		{
			Point mean;

			for (auto p = begin(); p != end(); ++p) {
				mean.x += p->x;
				mean.y += p->y;
				mean.t += p->t;
			}

			mean.x /= pointsCount();
			mean.y /= pointsCount();
			mean.t /= static_cast<PathTimeType>(pointsCount());

			return mean;
		}

    protected:

		// Stuff related to serialization.
	private:

		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			SERIALIZATION_LOG("PointCollection begins!"); 

			ar & bss::make_nvp("PointCollectionBase", bss::base_object<PointCollectionBase<Point>>(*this)); // because of template
			
			ar & SERIALIZATION_NVP(category);
			ar & SERIALIZATION_NVP(points);

			SERIALIZATION_LOG("PointCollection ends!"); 
		}
    };

};

BOOST_CLASS_EXPORT_KEY(motion_analysis::PointCollection);
BOOST_CLASS_TRACKING(motion_analysis::PointCollection, boost::serialization::track_never);