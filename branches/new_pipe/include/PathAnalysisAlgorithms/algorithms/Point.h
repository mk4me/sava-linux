#pragma once
#include <iterator>
#include <list>

#include "aux_serialization.h"

namespace motion_analysis {

    /// Point storage precision.
    typedef float PathPositionType;
    typedef int PathTimeType;

    /// <summary>
    /// Represents a single point of path or cluster.
    /// </summary>
    struct Point
    {
        /// x coordinate of path
        PathPositionType x;

        /// y coordinate of path
        PathPositionType y;

        /// time coordinate of path
        PathTimeType t;

        /// tau x coordinate of path
        PathPositionType tau_x;

        /// tau y coordinate of path
        PathPositionType tau_y;

        /// x coordinate of forward-filtered path (it does not need to be recalculated every time)
        PathPositionType tau_x_fov;

        /// y coordinate of forward-filtered path (it does not need to be recalculated every time)
        PathPositionType tau_y_fov;

		PathPositionType dx;
		PathPositionType dy;

		PathPositionType dx_fov;
		PathPositionType dy_fov;

        /// number of paths contributing to the cluster point (used only in clusters, in paths it is always equal to 1).
        int n;

		/// <summary>
        /// Resets all members.
        /// </summary>
		Point() : x(0), y(0), t(0), tau_x(0), tau_y(0), tau_x_fov(0), tau_y_fov(0), n(0) {}

		/// <summary>
		/// Initialises x,y,t members and resets others.
		/// </summary>
		Point(PathPositionType x, PathPositionType y, PathTimeType t, int n) : x(x), y(y), t(t), n(n), tau_x(0), tau_y(0), tau_x_fov(0), tau_y_fov(0) {}

        /// <summary>
        /// Calculates distance between two points.
        /// </summary>
        /// <param name=const Point & a>First point.</param>
        /// <param name=const Point & b>Second point.</param>
        /// <return>Calculated distance.</return>
        inline static const PathPositionType distance( const Point& a, const Point& b )
        {
            auto dx = a.x - b.x;
            auto dy = a.y - b.y;
            return sqrt( dx*dx + dy*dy );
        }

		/// <summary>
		/// Calculates distance between two points.
		/// </summary>
		/// <param name=const Point & a>First point.</param>
		/// <param name=const Point & b>Second point.</param>
		/// <return>Calculated distance.</return>
		inline static const PathPositionType squaredDistance( const Point& a, const Point& b )
		{
			auto dx = a.x - b.x;
			auto dy = a.y - b.y;
			return dx*dx + dy*dy;
		}

	// Stuff related to serialization.
	private:

		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & SERIALIZATION_NVP(x);
			ar & SERIALIZATION_NVP(y);
			ar & SERIALIZATION_NVP(t);
			ar & SERIALIZATION_NVP(tau_x);
			ar & SERIALIZATION_NVP(tau_y);
			ar & SERIALIZATION_NVP(tau_x_fov);
			ar & SERIALIZATION_NVP(tau_y_fov);
			ar & SERIALIZATION_NVP(n);
		}
    };

}