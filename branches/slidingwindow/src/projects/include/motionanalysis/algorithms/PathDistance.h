#pragma once
#include "Path.h"
#include <memory>
#include "PathClusteringParams.h"

namespace motion_analysis {

    /// Forward declaration of cluster class.
    class PathCluster;

    /// <summary>
    /// Distance between cluster and path.
    /// </summary>
    class PathDistance
    {
        public:
        /// Maximum scalar value.
        static float MAX_SCALAR;

        /// Maximum point value.
        static cv::Point_<float> MAX_POINT;

        /// Total distance component.
        PathPositionType total;

        /// Positional component.
        // Since 18.07.2012 deprecated - use X and Y components instead;
        //PathPositionType positionalComponent;

        /// Positional dispertion (beta parameter).
        cv::Point_<PathPositionType> dispertion;

        /// Positional squared dispertion.
        cv::Point_<PathPositionType> squaredDispertion;

        /// Tangential component
        PathPositionType tangentialComponent;

        /// Overlap length
        int length;
		
        /// <summary>
        /// Calculates distance between a cluster and a path. It is based on location component and tangential component.
        /// If path and cluster are of different categories, distance is infinite.
        /// </summary>
        /// <param name=cluster>Cluster to be considered.</param>
        /// <param name=path>Path to be considered.</param>
        PathDistance(
            const motion_analysis::PathCluster& cluster,
            const motion_analysis::Path& path,
            const motion_analysis::PathClusteringParams& params );

        /// <summary>
        /// Calculates distance between two clusters. It is based on location component and tangential component.
        /// Clusters can be of different categories.
        /// </summary>
        /// <param name=cluster>First cluster.</param>
        /// <param name=path>Second cluster.</param>
        /// <returns>Distance between clusters.</returns>
        PathDistance(
            const motion_analysis::PathCluster& cluster1,
            const motion_analysis::PathCluster& cluster2,
            const motion_analysis::PathClusteringParams& params );

        /// <summary>
        /// Checks whether distance meets conditions described by parameters.
        /// </summary>
        /// <param name=thresholds>Structure with threshold values.</param>
        /// <returns>Flag indicating whether distance is in desired bounds.</returns>
        bool checkConditions( const motion_analysis::DistanceThresholds& thresholds );

    protected:
		/// <summary>
		/// Creates uninitialized object.
		/// </summary>
		PathDistance();

        /// <summary>
        /// Sets distance to its maximum value.
        /// </summary>
        /// <returns></returns>
        void setToMax();

	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & SERIALIZATION_NVP(total);
			ar & SERIALIZATION_NVP(dispertion);
			ar & SERIALIZATION_NVP(squaredDispertion);
			ar & SERIALIZATION_NVP(tangentialComponent);
		}

    };

}