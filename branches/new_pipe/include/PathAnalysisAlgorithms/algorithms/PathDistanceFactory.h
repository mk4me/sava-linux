#pragma once
#include <memory>

namespace motion_analysis
{
	class PathDistance;
	class PathCluster;
	class Path;
	class PathClusteringParams;

	/// <summary>
	/// Creates PathDistance objects. Static member factory could be changed to change object creation algorithm. 
	/// PathDistanceFactory is default factory.
	/// </summary>
	class PathDistanceFactory
	{
	public:
		/// <summary>
		/// Creates distance between cluster and path.
		/// </summary>
		/// <param name=cluster>Cluster</param>
		/// <param name=path>Path</param>
		/// <param name=params>Parameters of path clustering algorithm</param>
		/// <returns>Created distance</returns>
		static std::shared_ptr<PathDistance> create(
			const motion_analysis::PathCluster& cluster,
			const motion_analysis::Path& path,
			const motion_analysis::PathClusteringParams& params)
		{
			return factory->createImpl(cluster, path, params);
		}

		/// <summary>
		/// Creates distance between two clusters.
		/// </summary>
		/// <param name=cluster1>First cluster</param>
		/// <param name=cluster2>Second cluster</param>
		/// <param name=params>Parameters of path clustering algorithm</param>
		/// <returns>Created distance</returns>
		static std::shared_ptr<PathDistance> create(
			const motion_analysis::PathCluster& cluster1,
			const motion_analysis::PathCluster& cluster2,
			const motion_analysis::PathClusteringParams& params)
		{
			return factory->createImpl(cluster1, cluster2, params);
		}

		/// <summary>
		/// Sets factory created from template type argument. Uses default constructor.
		/// </summary>
		template<typename T>
		static void setFactory()
		{
			setFactory(std::make_shared<T>());
		}
		/// <summary>
		/// Sets factory from argument.
		/// </summary>
		/// <param name=f>Factory to set</param>
		static void setFactory(const std::shared_ptr<PathDistanceFactory>& f)
		{
			factory = f;
		}

	public:
		virtual std::shared_ptr<PathDistance> createImpl(
			const motion_analysis::PathCluster& cluster,
			const motion_analysis::Path& path,
			const motion_analysis::PathClusteringParams& params);

		virtual std::shared_ptr<PathDistance> createImpl(
			const motion_analysis::PathCluster& cluster1,
			const motion_analysis::PathCluster& cluster2,
			const motion_analysis::PathClusteringParams& params);

	private:
		static std::shared_ptr<PathDistanceFactory> factory;
	};
}
