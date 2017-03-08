#pragma once
#include <list>
#include <functional>
#include "Path.h"
#include "IClustering.h"
#include "PathCluster.h"
#include "PathClusteringParams.h"
#include "PathAssignments.h"

namespace motion_analysis {

	namespace multitarget_tracker
	{
		class CTracker;
	}

    /// <summary>
    /// Stores information about clustering (set of clusters together with assignments of paths to clusters).
    /// </summary>
	class PathClustering : public IClustering, public std::enable_shared_from_this<PathClustering>
    {
    public:
		/// Object embedding all clustering parameters.
        PathClusteringParams params;

		/// Cluster count
		virtual const size_t clusterCount() const { return clusters.size(); }

        /// Gets collection of paths.
        const std::map<uint64_t, std::shared_ptr<Path>> & getPaths() const { return paths; }

        /// Gets collection of clusters.
        const std::list<std::shared_ptr<PathCluster>> & getClusters() const { return clusters; }

		/// Gets collection of past clusters.
		const std::list<std::shared_ptr<PathCluster>> & getFinishedClusters() const { return finishedClusters; }

		const std::list<std::shared_ptr<PathCluster>> & getAlmostFinishedClusters() const { return almostFinishedClusters; }

        /// <summary>
        /// Sets initial values for clustering parameters.
        /// </summary>
        PathClustering();

		/// <summary>
		/// Loads path clustering object from a file. It does not destroy existing clusters, thus can be used to load results from multiple files.
		/// </summary>
		/// <param name=filename>Filename.</param>
		virtual void load(std::string filename, int marker);

		/// <summary>
		/// Saves path clustering object to a file.
		/// </summary>
		/// <param name=filename>Filename.</param>
		virtual void save(std::string filename);

        /// <summary>
        /// Lengthens given path by specified point. If path does not exists, it is created. Current time value is used as time coordinate
        /// </summary>
        /// <param name=id>Path identifier.</param>
		/// <param name=category>Path category. Only paths frome same categories can be clustered.</param>
        /// <param name=x>x coordinate of path point.</param>
        /// <param name=y>y coordinate of path point.</param>
        virtual void insertPath( uint64_t id, int category, PathPositionType x, PathPositionType y );

        /// <summary>
        /// Removes path with given identifier.
        /// </summary>
        /// <param name=id>Path identifier.</param>
        virtual void removePath( uint64_t id );

        /// <summary>
        /// Update clustering on the basis of current paths collection.
        /// </summary>
        virtual void update();

        /// <summary>
        /// Resets entire clustering.
        /// </summary>
        virtual void reset();

		/// <summary>
		/// Flushes finished clusters.
		/// </summary>
		virtual void resetFinishedClusters();

		/// <summary>
		/// Joins current clustering with other one.
		/// </summary>
		//virtual void merge(const PathClustering& other);

		/// Calculates memory usage
		const ::size_t computeMemoryUsage();

		const ::size_t calculateHash() const;
		
		void saveFinishedClustersWithDominantPoints();

		bool backInTime(PathTimeType newTime);

		void saveState();

		void refreshSplittedClusters();

		std::shared_ptr<PathCluster> getAssignedCluster(unsigned long pathId) const
		{
			auto foundIt = assignments.find(pathId);
			if (foundIt != assignments.end())
			{
				// mkloszczyk: return foundIt->second;
				return foundIt->second.cluster;
			}
			return std::shared_ptr<PathCluster>();
		}
		PathTimeType getAssignedTime(unsigned long pathId) const
		{
			auto foundIt = assignments.find(pathId);
			if (foundIt != assignments.end())
			{
				// mkloszczyk: return foundIt->second;
				return foundIt->second.time;
			}
			return PathTimeType(-1);
		}
		std::shared_ptr<PathDistance> getAssignedDistance(unsigned long pathId) const
		{
			auto foundIt = assignments.find(pathId);
			if (foundIt != assignments.end())
			{
				// mkloszczyk: return foundIt->second;
				return foundIt->second.distance;
			}
			return std::shared_ptr<PathDistance>();
		}

		void processAlmostFinishedClusters();

    protected:
        /// Number of finished paths from algorithm start.
		int finishedPathCount;

		/// Total rejections count.
		int shortPathCount;

		/// Average path length.
		float avgPathLength;

		// Flag indicating whether clustering is up to date.
        bool upToDate;

        /// Collection of currently existing paths. It should be updated in each frame.
        std::map<uint64_t, std::shared_ptr<Path>> paths;

        /// Collection of currently existing clusters.
        std::list<std::shared_ptr<PathCluster>> clusters;

		std::map<int, size_t> pathLengthsHistogram;

		std::map<int, size_t> clusterLengthsHistogram;

        /// Assignments of currently existing paths ids to clusters.
		// mkloszczyk: std::map<uint64_t, std::shared_ptr<PathCluster>> assignments;
		PathAssignments assignments;

		/// Collection of past clusters.
		std::list<std::shared_ptr<PathCluster>> finishedClusters;

		std::list<std::shared_ptr<PathCluster>> almostFinishedClusters;

		unsigned long long lastMovingPathId;

		/// Performs assignment of newly introduced paths.
		virtual void doAssignment();

		/// Performs clusters merging.
		virtual void doMerging();

		virtual void doMergingGaps();

		/// Performs moving paths to between clusters.
		virtual void doPathMoving();

		/// Performs moving paths by removing cluster and assign all paths once again.
		virtual void doClusterRemoving();

		//virtual void doPathSplitting();

		//virtual void doMultitargetTracking();

		/// Calculate distances between path and existing clusters and choose closest one
		void findClosestCluster(const std::shared_ptr<Path>& path, std::shared_ptr<PathCluster>& closestCluster, std::shared_ptr<PathDistance>& minDistance);

	private:

		friend class boost::serialization::access;
		
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{	
			SERIALIZATION_LOG("PathClustering begins!");
			
			// serialise only finished clusters and clustering parameters
			ar & SERIALIZATION_BASE_OBJECT_NVP(IClustering);
			
			ar & SERIALIZATION_NVP(finishedClusters);
			ar & SERIALIZATION_NVP(params);

			SERIALIZATION_LOG("PathClustering ends!");
		}
		static const std::vector<Point> ms_EmptyPoints;

		struct State
		{
			State() : valid(false) {}
			std::string buffer;
			PathTimeType time;
			bool valid;
		};
		void saveState(State& state);
		bool loadState(const State& state);

		std::vector<State> stateHistory;


	};
}

BOOST_CLASS_EXPORT_KEY(motion_analysis::PathClustering);
BOOST_CLASS_TRACKING(motion_analysis::PathClustering, boost::serialization::track_selectively);