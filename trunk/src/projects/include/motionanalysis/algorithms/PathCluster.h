#pragma once
#include <memory>
#include <list>
#include "Path.h"
#include "PathDistance.h"
#include "PointCollection.h"
#include "ClusterBase.h"
#include "PathClusteringParams.h"


namespace motion_analysis {

    /// <summary>
    /// Represents a path cluster.
    /// </summary>
    class PathCluster : public PointCollection, public ClusterBase
    {
        public:

		/// Enables forward filter in update
		static bool do_forward_filter_in_update;
	
		static int currentId;

		/// Path identifier.
		int id;

		/// Mean displacement of paths contributing to the cluster.
        cv::Point_<PathPositionType> b;

        /// Variance of displacement of paths contributing to the cluster.
        cv::Point_<PathPositionType> bb;

        /// Total number of paths contributing to a cluster including removed paths.
        int bk;

		/// Total number of paths contributing to a cluster at given moment.
		int current_bk;

		/// Maximum displacement of path points contributing to the cluster. Needed for motion visualisation (focused view).
		cv::Point_<PathPositionType> max_b; 

		/// Collection of elements contributing to the cluster.
		std::list<std::shared_ptr<Path>> elements;

		std::list<std::pair<Point, Point>> mergedGaps;

		/// Gets number of elements in path cluster.
		virtual const ::size_t elementsCount() const { return elements.size(); }

		/// <summary>
		/// Gets cluster mean.
		/// </summary>
		Point getMean() const;

        /// <summary>
        /// Creates cluster with path assigned to it.
        /// </summary>
        /// <param name=path>Path to be initially assigned.</param>
        PathCluster( std::shared_ptr<Path> & path );

		/// <summary>
		/// Virtual destructor.
		/// </summary>
		virtual ~PathCluster() {}

        /// <summary>
        /// Updates cluster on the basis of some path already assigned to it.
        /// </summary>
        /// <param name=path>Path already assigned to the cluster.</param>
		/// <param name=params>Clustering parameters.</param>
        void update( const std::shared_ptr<const Path> & path, const PathClusteringParams& params );

        /// <summary>
        /// Marks path as finished.
        /// </summary>
        /// <param name=path>Path to be marked as finished.</param>
        /// <returns>Flag indicating whether cluster is empty.</returns>
        bool markPathEnd( const std::shared_ptr<Path> & path );

		/// <summary>
		/// Removes specified path from cluster.
		/// </summary>
		/// <param name=path>Path to be removed.</param>
		/// <returns>Flag indicating whether cluster is empty.</returns>
		bool removePath( const std::shared_ptr<Path> & path );

        /// <summary>
        /// Assigns new path to the cluster. It modifies c member of the path.
        /// </summary>
        /// <param name=path>Path to be assigned to cluster.</param>
        /// <param name=distance>Distance structure.</param>
        /// <param name=params>Clustering parameters.</param>
        void assign( std::shared_ptr<Path> & path, const PathDistance& distance, const PathClusteringParams& params );

        /// <summary>
        /// Tries to assign new path to the cluster. It does not alter neither cluster nor the path.
        /// It only creates shallow copy of cluster with updated parameters.
        /// </summary>
        /// <param name=path>Path to be assigned to cluster.</param>
        /// <param name=dispertion>Dispertion parameter for distance estimation (beta).</beta>
        /// <returns>Shallow copy of existing cluster with updated parameters.</returns>
        std::shared_ptr<PathCluster> tryAssign( const Path& path, const cv::Point_<PathPositionType>& dispertion ) const;

        /// <summary>
        /// Merges cluster with some other cluster.
        /// </summary>
        /// <param name=cluster>Cluster to be joined with.</param>
		/// <param name=distance>Distance between clusters.</param>
		/// <param name=params>Clustering parameters.</param>
        void merge( const PathCluster& cluster, const PathDistance& distance, const PathClusteringParams& params );

        /// <summary>
        /// Tries to merge cluster with some other cluster.
        /// </summary>
        /// <param name=cluster>Cluster to be joined with.</param>
        std::shared_ptr<PathCluster> tryMerge( const PathCluster& cluster ) const;

		/// Calculates memory usage.
		const ::size_t computeMemoryUsage();

		std::shared_ptr<PathCluster> enlarge(PathTimeType pointsNum, bool front = false, bool back = true) const;


		const ::size_t calculateHash() const;
		
		void addMergedGap(const Point& a, const Point& b);
		void addMergedGaps(const std::list<std::pair<Point, Point>>& gaps);

		cv::Rect getContour(PathTimeType time, const cv::Size& margin = cv::Size()) const;

protected:
		/// <summary>
		/// Non-parametric constructor for serialization.  
		/// </summary>
		PathCluster() : PointCollection(), ClusterBase() {};
			
		/// <summary>
        /// Copy constructor (it copies everything but paths collection).
        /// </summary>
        /// <param name=path>Reference cluster.</param>
        PathCluster( const PathCluster& ref );

		
	private:


		// Stuff related to serialization.
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			SERIALIZATION_LOG("PathCluster begins!"); 
			
			ar & SERIALIZATION_BASE_OBJECT_NVP(PointCollection);
			ar & SERIALIZATION_BASE_OBJECT_NVP(ClusterBase);
		
			ar & SERIALIZATION_NVP(id);
			ar & SERIALIZATION_NVP(b);
			ar & SERIALIZATION_NVP(bb);
			ar & SERIALIZATION_NVP(bk);
			ar & SERIALIZATION_NVP(max_b);
			ar & SERIALIZATION_NVP(current_bk);
			ar & SERIALIZATION_NVP(elements);
			
			SERIALIZATION_LOG("PathCluster ends!"); 
		}
    };

}

BOOST_CLASS_EXPORT_KEY(motion_analysis::PathCluster);
BOOST_CLASS_TRACKING(motion_analysis::PathCluster, track_selectively);