#pragma once
#include "PathDistance.h"

namespace motion_analysis {

	/// Forward declaration of cluster class.
	//class PathCluster;

	/// <summary>
	/// Distance between cluster and path.
	/// </summary>
	class OptimizedPathDistance : public PathDistance
	{
	public:
		/// <summary>
		/// Calculates distance between a cluster and a path. It is based on location component and tangential component.
		/// If path and cluster are of different categories, or if squaredDispertion exceeds threshold, distance is infinite.
		/// </summary>
		/// <param name=cluster>Cluster to be considered.</param>
		/// <param name=path>Path to be considered.</param>
		OptimizedPathDistance(
			const motion_analysis::PathCluster& cluster,
			const motion_analysis::Path& path,
			const motion_analysis::PathClusteringParams& params);

		/// <summary>
		/// Calculates distance between two clusters. It is based on location component and tangential component.
		/// Clusters can be of different categories. If squaredDispertion exceeds threshold, distance is infinite.
		/// </summary>
		/// <param name=cluster>First cluster.</param>
		/// <param name=path>Second cluster.</param>
		/// <returns>Distance between clusters.</returns>
		OptimizedPathDistance(
			const motion_analysis::PathCluster& cluster1,
			const motion_analysis::PathCluster& cluster2,
			const motion_analysis::PathClusteringParams& params);


	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & SERIALIZATION_BASE_OBJECT_NVP(PathDistance);
		}

	};

}