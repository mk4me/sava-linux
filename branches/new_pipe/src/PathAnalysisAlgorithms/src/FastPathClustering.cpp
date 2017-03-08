#include "PathAnalysisAlgorithms/algorithms/FastPathClustering.h"
#include <utility>
#include <omp.h>
#include <unordered_map>
#include <boost/atomic.hpp>
#include "PathAnalysisAlgorithms/algorithms/PathDistanceFactory.h"

using namespace motion_analysis;
//using namespace utils;

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::FastPathClustering::doAssignment()
{
	boost::atomic<unsigned int> updatesCount(0);
	size_t assignmentsCount = 0;
	size_t creationsCount = 0;
	boost::atomic<unsigned int> rejectionsCount(0);
	
	// copy paths to vector to use in OpenMP for loop
	std::vector<std::pair<unsigned long long, std::shared_ptr<Path>>> pathsVector(paths.begin(), paths.end());
	

	typedef std::pair<std::shared_ptr<Path>, std::shared_ptr<PathCluster>> PathClusterPair;
	typedef std::tuple<std::shared_ptr<Path>, std::shared_ptr<PathCluster>, std::shared_ptr<PathDistance>> PathClusterDistanceTuple;
	typedef std::vector<PathClusterPair> PathClusterVector;
	typedef std::vector<PathClusterDistanceTuple> PathClusterDistanceVector;
	typedef std::map<std::shared_ptr<PathCluster>, std::vector<std::shared_ptr<Path>>> ClusterToPathMap;
	
	typedef std::vector<PathClusterVector> PathClusterVectorVector;

	PathClusterVectorVector pathClusterVectorVector(omp_get_max_threads());
	ClusterToPathMap clusterToUpdate;
	PathClusterDistanceVector clusterToAssign;
	PathClusterVector clusterToCreate;	
	
	// iterate through all paths
#pragma omp parallel for schedule(dynamic)
	for(int i=0; i<static_cast<int>(pathsVector.size()); ++i) {
		// make an alias for pointer to current path
		auto & path = pathsVector[i].second;

		/// check if path length criteria is met
		if( path->pointsCount() < params.minimumPathLength ) {
			++rejectionsCount;
			continue;
		}
		// check path assignment
		auto assignmentIt = assignments.find( path->id );
		if( assignmentIt != assignments.end() ) {
			// if current path is already assigned to some cluster, update cluster
			// mkloszczyk: auto & cluster = assignmentIt->second;
			auto & cluster = assignmentIt->second.cluster;
			pathClusterVectorVector[omp_get_thread_num()].push_back(std::make_pair(path, cluster));
		} else {
			// if current path is not assigned to any cluster try to assign it to some existing one;
			// calculate distances between path and existing clusters and choose closest one
			std::shared_ptr<PathDistance> minDistance;
			std::shared_ptr<PathCluster> closestCluster;
			findClosestCluster(path, closestCluster, minDistance);

			if( closestCluster.get() != NULL ) {
				// assign to the closest cluster		
#pragma omp critical (doAssignment_ClusterAssing)
				clusterToAssign.push_back(std::make_tuple(path, closestCluster, minDistance));
			} else {
				// create a new cluster containing single path and add it to collection
				std::shared_ptr<PathCluster> cluster( new PathCluster( path ) );
#pragma omp critical (doAssignment_ClusterCreate)
				clusterToCreate.push_back(std::make_pair(path, cluster));
			}
		}
	}
	
	for (auto it = pathClusterVectorVector.begin(); it != pathClusterVectorVector.end(); ++it)
	{
		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{			
			clusterToUpdate[it2->second].push_back(it2->first);
		}
	}

	// copy paths to vector to use in OpenMP for loop
	std::vector<std::pair<std::shared_ptr<PathCluster>, std::vector<std::shared_ptr<Path>>>> clusterToUpdateVec(clusterToUpdate.begin(), clusterToUpdate.end());
#pragma omp parallel for schedule(dynamic)
	for (int i=0; i<static_cast<int>(clusterToUpdateVec.size()); ++i)
	{
		auto & cluster = clusterToUpdateVec[i].first;
		auto & paths = clusterToUpdateVec[i].second;
		for (auto it = paths.begin(); it != paths.end(); ++it)
		{
			cluster->update(*it, params);
			++updatesCount;
		}
	}

	creationsCount += clusterToCreate.size();
	for (auto it = clusterToCreate.begin(); it != clusterToCreate.end(); ++it)
	{
		clusters.push_back(it->second);
		// mkloszczyk: assignments.insert(std::map<unsigned long, std::shared_ptr<PathCluster>>::value_type(it->first->id, it->second));
		assignments.insert(it->first->id, it->second, this->time, PathDistanceFactory::create(*it->second, *it->first, params));
	}
	assignmentsCount += clusterToAssign.size();
	for (auto it = clusterToAssign.begin(); it != clusterToAssign.end(); ++it)
	{
		std::get<1>(*it)->assign(std::get<0>(*it), *std::get<2>(*it), params);
		// mkloszczyk: assignments.insert(std::map<unsigned long, std::shared_ptr<PathCluster>>::value_type(std::get<0>(*it)->id, std::get<1>(*it)));
		assignments.insert(std::get<0>(*it)->id, std::get<1>(*it), this->time, PathDistanceFactory::create(*std::get<1>(*it), *std::get<0>(*it), params));
	}

	assert( paths.size() == rejectionsCount + updatesCount + assignmentsCount + creationsCount );

	STATS_WRITE("count-path assignments", assignmentsCount);
	STATS_WRITE("count-path cluster creations", creationsCount);
	STATS_WRITE("count-path cluster updates", updatesCount);
	STATS_WRITE("count-path rejections", rejectionsCount);

	STATS_ADD("count-total path assignments", assignmentsCount);
	STATS_ADD("count-total path cluster creations", creationsCount);
	STATS_ADD("count-total path cluster updates", updatesCount);
	STATS_ADD("count-total path rejections", rejectionsCount);
}