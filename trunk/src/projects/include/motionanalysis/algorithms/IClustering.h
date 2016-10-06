#pragma once
#include "Path.h"
#include "utils/StatisticsProvider.h"

namespace motion_analysis
{
	class IClustering : public StatisticsProvider
	{
	public:
		/// Gets current time value.
		const PathTimeType getTime() const { return time; }

		/// Sets current time value.
		void setTime( PathTimeType v ) { time = v; }
		
		/// Cluster count
		virtual const size_t clusterCount() const = 0;

		IClustering() : time(0) {}


		virtual ~IClustering () {}

		/// <summary>
		/// Lengthens given path by specified point. If path does not exists, it is created. Current time value is used as time coordinate
		/// </summary>
		/// <param name=id>Path identifier.</param>
		/// <param name=category>Path category. Only paths frome same categories can be clustered.</param>
		/// <param name=x>x coordinate of path point.</param>
		/// <param name=y>y coordinate of path point.</param>
		virtual void insertPath( uint64_t, int category, PathPositionType x, PathPositionType y ) = 0;

		/// <summary>
		/// Removes path with given identifier.
		/// </summary>
		/// <param name=id>Path identifier.</param>
		virtual void removePath( uint64_t id ) = 0;

		/// <summary>
		/// Update clustering on the basis of current paths collection.
		/// </summary>
		virtual void update() = 0;

		/// <summary>
		/// Resets whole clustering.
		/// </summary>
		virtual void reset() = 0;

		/// Calculates memory usage
		virtual const ::size_t computeMemoryUsage() = 0;

		virtual void print(std::ostream& out) const {}
	
	protected:
		/// Current time point.
		PathTimeType time;

		// Stuff related to serialization.
	private:

		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			SERIALIZATION_LOG("IClustering begins!");
			ar & SERIALIZATION_NVP(time);
			SERIALIZATION_LOG("ICLustering ends!");
		}
	};
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(motion_analysis::IClustering);
