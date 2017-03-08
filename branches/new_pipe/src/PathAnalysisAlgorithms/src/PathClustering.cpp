#include <functional>
#include <memory>
#include <fstream>
#include "PathAnalysisAlgorithms/algorithms/aux_serialization.h"
#include "PathAnalysisAlgorithms/algorithms/PathClustering.h"
#include "PathAnalysisAlgorithms/algorithms/PathDistance.h"
#include "PathAnalysisAlgorithms/algorithms/PathDistanceFactory.h"
#include "PathAnalysisAlgorithms/algorithms/auxmath.h"
#include "utils/timer.h"

#include <set>

#define REMOVE_CLUSTER_WHEN_FINISHED 1


BOOST_CLASS_EXPORT_IMPLEMENT(motion_analysis::PathClustering);

using namespace motion_analysis;
using namespace  boost::serialization;

const std::vector<motion_analysis::Point> motion_analysis::PathClustering::ms_EmptyPoints;

/// <summary>
/// See declaration for all the details.
/// </summary>
motion_analysis::PathClustering::PathClustering() : IClustering()
{
    upToDate = true;
    time = 0;
	finishedPathCount = 0;
	avgPathLength = 0;
	shortPathCount = 0;
	lastMovingPathId = 0;

	pathLengthsHistogram[5] = 0;
	pathLengthsHistogram[10] = 0;
	pathLengthsHistogram[20] = 0;
	pathLengthsHistogram[50] = 0;
	pathLengthsHistogram[100] = 0;
	pathLengthsHistogram[1000] = 0;

	clusterLengthsHistogram[5] = 0;
	clusterLengthsHistogram[10] = 0;
	clusterLengthsHistogram[20] = 0;
	clusterLengthsHistogram[50] = 0;
	clusterLengthsHistogram[100] = 0;
	clusterLengthsHistogram[1000] = 0;

	statistics["time-total-assignment-ms"] = 0;
	statistics["time-total-merging-ms"] = 0;
	
}


/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::save( std::string filename )
{
	std::ofstream ofs(filename, std::ios::binary);
	boost::archive::binary_oarchive oa(ofs);	

	oa.register_type<ParamWrapper<int>>(); 
	oa.register_type<ParamWrapper<float>>(); 
	
	/// save the object itself, not the pointer
	oa & *this;

	//Path* p = this->paths.begin()->second.get();
	//oa & p;
}


/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::load(std::string filename, int marker)
{
	std::ifstream ifs;
	
	ifs.open(filename, std::ios::binary);
	boost::archive::binary_iarchive ia(ifs);

	ia.register_type<ParamWrapper<int>>(); 
	ia.register_type<ParamWrapper<float>>();
	ia.register_type<PathClustering>();
	
	PathClustering clustering;
	ia & clustering;
	
	marker <<= 27;

	// fixme:
	for (auto c = clustering.getFinishedClusters().begin(); c != clustering.getFinishedClusters().end(); ++c) {
		(**c).id |= marker;
	}

	this->finishedClusters.insert(finishedClusters.end(), clustering.getFinishedClusters().begin(), clustering.getFinishedClusters().end());	
	
	// fixme:
/*	int i = 0;
	std::cout << "hash = " << calculateHash() << ", count = " << finishedClusters.size() << std::endl;
	for (auto it = finishedClusters.begin(); it != finishedClusters.end(); ++it, ++i) {
		std::cout << "cluster[" << i << "]: " << (**it).elements.size() << std::endl;
	}
*/
	//Path *p;
	//ia & p;
}

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::insertPath( uint64_t id, int category, PathPositionType x, PathPositionType y )
{
    this->upToDate = false;
    std::shared_ptr<Path> path;

    // create path or take it from collection
    if( paths.find( id ) == paths.end() ) {
        paths[id] = path = std::shared_ptr<Path>( new Path( id, category ) );

    } else {
        path = paths[id];
    }

    // append point to path
    path->append( x, y, time, params.alpha );
	
    assert( path->category == category );
}

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::removePath( uint64_t id )
{
    this->upToDate = false;
    auto pair = paths.find( id );

    if( pair != paths.end() ) {

		auto & path = pair->second;
		uint64_t pid = path->id;

	//	path->clearTangentialData();
		avgPathLength = ((avgPathLength * finishedPathCount) + path->pointsCount()) / (finishedPathCount + 1);
		
		++finishedPathCount;
		if (path->pointsCount() < params.minimumPathLength ) {
			++shortPathCount;
		}

        // remove assignment to cluster and cluster if empty
        if( assignments.find( path->id ) != assignments.end() ) {
			// mkloszczyk: auto & cluster = assignments[path->id];
			auto & cluster = assignments.getCluster(path->id);
            
			// inform cluster that path has finished
            if( cluster->markPathEnd(path) ) {
				
                // move to repository and remove from main collection
				//	cluster->clearTangentialData();
				almostFinishedClusters.push_back(cluster);
				
				for (auto& e : clusterLengthsHistogram) {
					if (cluster->pointsCount() < e.first) {
						++e.second;
						break;
					}
				}
				clusters.remove(cluster);
            }

            assignments.erase( path->id );
        }
		
        // remove from current collection
		for (auto& e : pathLengthsHistogram) {
			if (path->pointsCount() < e.first) {
				++e.second;
				break;
			}
		}

        paths.erase( path->id );
    }
}

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::reset()
{
    std::list<std::shared_ptr<PathCluster>>().swap( clusters );
	std::list<std::shared_ptr<PathCluster>>().swap( finishedClusters );
	std::list<std::shared_ptr<PathCluster>>().swap( almostFinishedClusters );
	// mkloszczyk: std::map<uint64_t, std::shared_ptr<PathCluster>>().swap(assignments);
	assignments.reset();
    std::map<uint64_t, std::shared_ptr<Path>>().swap( paths );

    this->upToDate = true;
	this->finishedPathCount = 0;
}

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::resetFinishedClusters()
{
	this->finishedClusters.clear();
}

int g_NextClusterId = 0;
std::map<std::shared_ptr<PathCluster>, int> g_ClusterToIdMap;

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::update()
{
	if( this->upToDate ) {
		return;
	}

	utils::timer mytimer;
	

	mytimer.start();
	doAssignment();
	mytimer.stop();
	statistics["time-assignment-ms"] = static_cast<StatisticsProvider::ValueType>(mytimer.getmsTick());
	statistics["time-total-assignment-ms"] += mytimer.getmsTick();
	
	mytimer.start();
	doPathMoving();
	mytimer.stop();
	statistics["time-moving-ms"] = static_cast<StatisticsProvider::ValueType>(mytimer.getmsTick());
	statistics["time-total-moving-ms"] += mytimer.getmsTick();
	
	mytimer.start();
	doMerging();
	mytimer.stop();
	statistics["time-merging-ms"] = static_cast<StatisticsProvider::ValueType>(mytimer.getmsTick());
	statistics["time-total-merging-ms"] += mytimer.getmsTick();
	
	mytimer.start();
	doClusterRemoving();
	mytimer.stop();
	statistics["time-removing-ms"] = static_cast<StatisticsProvider::ValueType>(mytimer.getmsTick());
	statistics["time-total-removing-ms"] += mytimer.getmsTick();
	
	mytimer.start();
	doMergingGaps();
	mytimer.stop();
	statistics["time-merging-gaps-ms"] = static_cast<StatisticsProvider::ValueType>(mytimer.getmsTick());
	statistics["time-total-merging-gaps-ms"] += mytimer.getmsTick();
	
	/*mytimer.start();
	doMultitargetTracking();
	mytimer.stop();
	statistics["time-multitarget-tracking-ms"] = static_cast<StatisticsProvider::ValueType>(mytimer.getmsTick());
	statistics["time-total-multitarget-tracking-ms"] += mytimer.getmsTick();	*/
	/*
	mytimer.start();
	doPathSplitting();
	mytimer.stop();
	statistics["time-path-splitting-ms"] = static_cast<StatisticsProvider::ValueType>(mytimer.getmsTick());
	statistics["time-total-path-splitting-ms"] += mytimer.getmsTick();
		*/
	//clusters.sort([](std::shared_ptr<Cluster> c1, std::shared_ptr<Cluster> c2)->bool { return c1->bk > c2->bk; });
	this->upToDate = true;

	/*std::ofstream currentClustersFile("F:\\Rynek_12h\\clusters\\clusters.txt", std::ios::app);
	currentClustersFile << this->time;
	if (!clusters.empty())
	{
		for (auto c = clusters.begin(); c != clusters.end(); ++c)
		{
			auto& cluster = *c;
			if (cluster->begin() == cluster->end())
				continue;

			auto it = g_ClusterToIdMap.find(cluster);
			if (it == g_ClusterToIdMap.end())
			{
				g_ClusterToIdMap[cluster] = g_NextClusterId++;
			}

			auto point = cluster->rbegin();			
			if (point->t == this->time)
			{
				currentClustersFile << " " << g_ClusterToIdMap[cluster] << " " << point->x << " " << point->y;
			}
		}		
	}
	currentClustersFile << std::endl;*/

	statistics["current-paths"] = static_cast<StatisticsProvider::ValueType>(paths.size());
	statistics["current-path clusters"] = static_cast<StatisticsProvider::ValueType>(clusters.size());
	statistics["finished-paths"] = static_cast<StatisticsProvider::ValueType>(finishedPathCount);
	statistics["finished-path clusters"] = static_cast<StatisticsProvider::ValueType>(finishedClusters.size());
	statistics["finished-path almost clusters"] = static_cast<StatisticsProvider::ValueType>(almostFinishedClusters.size());
	statistics["finished-average path length"] = avgPathLength;
	statistics["finished-short path count"] = static_cast<StatisticsProvider::ValueType>(shortPathCount);

	for (auto &e : pathLengthsHistogram) {
		std::string key = "histogram-path-" + std::to_string(e.first);
		statistics[key] = (double)e.second;
	}

	for (auto &e : clusterLengthsHistogram) {
		std::string key = "histogram-cluster-" + std::to_string(e.first);
		statistics[key] = (double)e.second;
	}
}


/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::doAssignment()
{
	int updatesCount = 0;
	int assignmentsCount = 0;
	int creationsCount = 0;
	int rejectionsCount = 0;
	
	// iterate through all paths
	for( auto p = paths.begin(); p != paths.end(); ++p ) {
		// make an alias for pointer to current path
		auto & path = p->second;

		/// check if path length criteria is met
		if( path->pointsCount() < params.minimumPathLength ) {
			++rejectionsCount;
			continue;
		}

		// check path assignment
		if( assignments.find( path->id ) != assignments.end() ) {
			// if current path is already assigned to some cluster, update cluster
			// mkloszczyk: auto & cluster = assignments[path->id];
			auto & cluster = assignments.getCluster(path->id);
			cluster->update( path, params );
			++updatesCount;

		} else {
			// if current path is not assigned to any cluster try to assign it to some existing one;
			// calculate distances between path and existing clusters and choose closest one
			std::shared_ptr<PathDistance> minDistance;
			std::shared_ptr<PathCluster> closestCluster;			
			findClosestCluster(path, closestCluster, minDistance);

			if( closestCluster.get() != NULL ) {
				// assign to the closest cluster
				closestCluster->assign( path, *minDistance, params );
				// mkloszczyk: assignments[path->id] = closestCluster;
				assignments.insert(path->id, closestCluster, time, minDistance);
				++assignmentsCount;
			} else {
				// create a new cluster containing single path and add it to collection
				std::shared_ptr<PathCluster> cluster = std::shared_ptr<PathCluster>( new PathCluster( path ) );
				clusters.push_back( cluster );
				// mkloszczyk: assignments[path->id] = cluster;
				assignments.insert(path->id, cluster, time, PathDistanceFactory::create(*cluster, *path, params));
				++creationsCount;
			}
		}
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

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::doMerging()
{
	int mergesCount = 0;

	if (params.enableMerging > 0 && this->time % params.enableMerging == 0 && clusters.size() >= 2) {
		// iterate through clusters and check for merges
		for( auto c1 = clusters.begin(); c1 != std::prev( clusters.end() ) && c1 != clusters.end(); ++c1 ) { // outer loop iterates from largest to smallest
			for( auto c2 = std::next( c1 ); c2 != clusters.end(); ++c2 ) { //
				auto distance = PathDistanceFactory::create(**c1, **c2, params);

				// check if distance bounds are met
				if( distance->checkConditions( params.clusterThresholds ) ) {
					// merge clusters and reassigns paths
					( **c1 ).merge( **c2, *distance, params );
					++mergesCount;

					for( auto p = ( **c2 ).elements.begin(); p != ( **c2 ).elements.end(); ++p ) {
						auto path = std::dynamic_pointer_cast<Path>(*p);
						// mkloszczyk: assignments[path->id] = *c1; // change assignment from c2 to c1
						assignments.change(path->id, *c1, time, PathDistanceFactory::create(**c1, *path, params)); // change assignment from c2 to c1
					}

					c2 = clusters.erase( c2 );

					if( c2 == clusters.end() ) {
						break;
					}
				}
			}

			// it may happen that after removing some clusters c1 will point to end
			if( c1 == clusters.end() ) {
				break;
			}
		}
	}
	
	STATS_WRITE("count-path cluster merges", mergesCount);
	STATS_ADD("count-total path cluster merges", mergesCount);
}

void motion_analysis::PathClustering::doMergingGaps()
{	
	int mergesCount = 0;
	int mergesFinishedCount = 0;
	int timeDistanceSum = 0;
	float xDistanceSum = 0;
	float yDistanceSum = 0;

	if (params.enableMergingGaps == 0)
	{
		processAlmostFinishedClusters();
		return;
	}
		
	if (this->time % params.enableMergingGaps == 0 && !almostFinishedClusters.empty()/* && !clusters.empty()*/)
	{
		PathTimeType minTime = this->time; // std::numeric_limits<PathTimeType>::max();
		for (auto c1 = clusters.begin(); c1 != clusters.end(); ++c1)
		{
			auto & cluster = *c1;
			auto a = cluster->points.begin();
			if (a->t < minTime)
				minTime = a->t;
		}
		minTime = std::max(this->time - params.mergingOldestClusterTime, std::max(0, minTime - params.mergingThresholds.time));

		for (auto c2 = almostFinishedClusters.begin(); c2 != almostFinishedClusters.end(); ++c2)
		{
			if ((*c2)->points.rbegin()->t < minTime)
			{
				finishedClusters.push_back(*c2);
				c2 = almostFinishedClusters.erase(c2);
				if (c2 == almostFinishedClusters.end()) {					
					break;
				}
				continue;
			}

			for (auto c1 = clusters.begin(); c1 != clusters.end(); ++c1)
			{				
				auto & A = (*c1)->points;
				auto a0 = A.begin();

				auto & B = (*c2)->points;
				auto bn = B.rbegin();

				PathTimeType d = a0->t - bn->t;
				
				if (d > 0 && d < params.mergingThresholds.time &&
					abs(a0->x - bn->x) < params.mergingThresholds.x &&
					abs(a0->y - bn->y) < params.mergingThresholds.y)
				{
					auto enlarged = (*c2)->enlarge(d);

					auto distance = PathDistanceFactory::create(**c1, *enlarged, params);

					// check if distance bounds are met
					if (distance->checkConditions(params.clusterThresholds)) {
												
						timeDistanceSum += d;
						xDistanceSum += abs(a0->x - bn->x);
						yDistanceSum += abs(a0->y - bn->y);

						// merge clusters and reassigns paths
						(**c1).addMergedGap(*a0, *bn);
						(**c1).merge(**c2, *distance, params);
						++mergesFinishedCount;

						for (auto p = (**c2).elements.begin(); p != (**c2).elements.end(); ++p) {
							auto path = std::dynamic_pointer_cast<Path>(*p);
							// mkloszczyk: assignments[path->id] = *c1; // change assignment from c2 to c1							
							assignments.change(path->id, *c1, time, PathDistanceFactory::create(**c1, *path, params)); // change assignment from c2 to c1
						}

						c2 = almostFinishedClusters.erase(c2);

						if (c2 == almostFinishedClusters.end()) {
							break;
						}
					}
				}
			}

			if (c2 == almostFinishedClusters.end()) {
				break;
			}
		}
	}
	
	if (params.enableMerging > 0 && this->time % params.enableMerging == 0 && clusters.size() >= 2) {
		// iterate through clusters and check for merges
		for (auto c1 = clusters.begin(); c1 != std::prev(clusters.end()) && c1 != clusters.end(); ++c1) { // outer loop iterates from largest to smallest
			for (auto c2 = std::next(c1); c2 != clusters.end(); ++c2) { //
								
				auto &A = (*c1)->points;
				auto &B = (*c2)->points;

				auto a0 = A.begin();
				auto an = A.rbegin();

				auto b0 = B.begin();
				auto bn = B.rbegin();

				PathTimeType d0 = b0->t - an->t;
				PathTimeType d1 = a0->t - bn->t;

				std::shared_ptr<PathCluster> firstCluster;
				std::shared_ptr<PathCluster> secondCluster;
				PathTimeType d;

				if (d0 > 0 && d0 < params.mergingThresholds.time)
				{
					// |---A---|  |---B---|
					firstCluster = *c1;
					secondCluster = *c2;
					d = d0;
				}
				else if (d1 > 0 && d1 < params.mergingThresholds.time)
				{
					// |---B---|  |---A---|
					firstCluster = *c2;
					secondCluster = *c1;
					d = d1;
				}
				else
					continue;

				if (abs(firstCluster->points.rbegin()->x - secondCluster->points.begin()->x) < params.mergingThresholds.x &&
					abs(firstCluster->points.rbegin()->y - secondCluster->points.begin()->y) < params.mergingThresholds.y)
				{
					auto enlarged = firstCluster->enlarge(d);
					auto distance = PathDistanceFactory::create(*enlarged, *secondCluster, params);

					// check if distance bounds are met
					if (distance->checkConditions(params.clusterThresholds)) {

						timeDistanceSum += d;
						xDistanceSum += abs(firstCluster->points.rbegin()->x - secondCluster->points.begin()->x);
						yDistanceSum += abs(firstCluster->points.rbegin()->y - secondCluster->points.begin()->y);

						// merge clusters and reassigns paths
						(**c1).addMergedGap(*secondCluster->points.begin(), *firstCluster->points.rbegin());
						(**c1).merge(**c2, *distance, params);
						++mergesCount;

						for (auto p = (**c2).elements.begin(); p != (**c2).elements.end(); ++p) {
							auto path = std::dynamic_pointer_cast<Path>(*p);
							// mkloszczyk: assignments[path->id] = *c1; // change assignment from c2 to c1
							assignments.change(path->id, *c1, time, PathDistanceFactory::create(**c1, *path, params)); // change assignment from c2 to c1
						}

						c2 = clusters.erase(c2);

						if (c2 == clusters.end()) {
							break;
						}
					}
				}
			}

			// it may happen that after removing some clusters c1 will point to end
			if (c1 == clusters.end()) {
				break;
			}
		}
	}

	STATS_WRITE("count-path cluster merges gaps (finished)", mergesFinishedCount);
	STATS_ADD("count-total path cluster merges gaps (finished)", mergesFinishedCount);
	STATS_WRITE("count-path cluster merges gaps", mergesCount);
	STATS_ADD("count-total path cluster merges gaps", mergesCount);

	STATS_WRITE("sum time-distance", timeDistanceSum);
	STATS_WRITE("sum x-distance", xDistanceSum);
	STATS_WRITE("sum y-distance", yDistanceSum);

	STATS_ADD("sum-total time-distance", timeDistanceSum);
	STATS_ADD("sum-total x-distance", xDistanceSum);
	STATS_ADD("sum-total y-distance", yDistanceSum);

	auto count = statistics["count-total path cluster merges gaps (finished)"];

	STATS_WRITE("avg-total time-distance", statistics["sum-total time-distance"] / count);
	STATS_WRITE("avg-total x-distance", statistics["sum-total x-distance"] / count);
	STATS_WRITE("avg-total y-distance", statistics["sum-total y-distance"] / count);
}

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::doPathMoving()
{
	int assignmentsCount = 0;
	int creationsCount = 0;
	int rejectionCount = 0;
	int notMovedCount = 0;

	if (params.enableMovingPaths > 0 && this->time % params.enableMovingPaths == 0 && clusters.size() > 1) {
		size_t pathsToExamine = std::min(std::max<size_t>(static_cast<size_t>(paths.size() * params.pathsPercentageMovesPerFrame), params.minPathsMovesPerFrame), paths.size());
		size_t i = 0;
		// iterate through all paths
		for (auto p = paths.lower_bound(lastMovingPathId); i < pathsToExamine; ++i, ++p) {
			
			if (p == paths.end())
				p = paths.begin();

			// make an alias for pointer to current path
			auto & path = p->second;

			lastMovingPathId = path->id + 1;

			/// check if path length criteria is met
			if (path->pointsCount() < params.minimumPathLength) {
				++rejectionCount;
				continue;
			}

			assert(assignments.find(path->id) != assignments.end());
			// mkloszczyk: auto & currentCluster = assignments[path->id];
			auto & currentCluster = assignments.getCluster(path->id);
			auto currentDistance = PathDistanceFactory::create(*currentCluster, *path, params);
			if (params.tryMovingOnlyWhenCurrentIsToFar && currentDistance->checkConditions(params.pathThresholds))
			{
				++notMovedCount;
				continue;
			}

			std::shared_ptr<PathDistance> minDistance;
			std::shared_ptr<PathCluster> closestCluster;			
			findClosestCluster(path, closestCluster, minDistance);
			
			// current cluster is the closest - change isn't nessesery
			if (closestCluster == currentCluster)
			{
				++notMovedCount;
				continue;
			}

			// if no cluster met distance threshold and creation flag is false
			if (!params.createNewCluster && closestCluster.get() == NULL)
			{
				++notMovedCount;
				continue;
			}
				
			// remove path from current cluster and cluster if it's empty.
			currentCluster->removePath(path);
#if REMOVE_CLUSTER_WHEN_FINISHED
			if (currentCluster->current_bk == 0) {
				finishedClusters.push_back(currentCluster);
				clusters.remove(currentCluster);
			}
#endif 

			if (closestCluster.get() != NULL) {
				// assign to the closest cluster
				closestCluster->assign(path, *minDistance, params);
				// mkloszczyk: assignments[path->id] = closestCluster;
				assignments.change(path->id, closestCluster, time, minDistance);
				++assignmentsCount;
			}
			else {
				// create a new cluster containing single path and add it to collection
				std::shared_ptr<PathCluster> cluster = std::make_shared<PathCluster>(path);
				clusters.push_back(cluster);
				// mkloszczyk: assignments[path->id] = cluster;
				assignments.change(path->id, cluster, time, PathDistanceFactory::create(*cluster, *path, params));
				++creationsCount;
			}
		}
	}

	STATS_WRITE("count path migration-moved", assignmentsCount);
	STATS_WRITE("count path migration-created", creationsCount);
	STATS_WRITE("count path migration-rejections", rejectionCount);
	STATS_WRITE("count path migration-notMoved", notMovedCount);

	STATS_ADD("count-total path migration-moved", assignmentsCount);
	STATS_ADD("count-total path migration-created", creationsCount);
	STATS_ADD("count-total path migration-rejections", rejectionCount);
	STATS_ADD("count-total path migration-notMoved", notMovedCount);
}

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::doClusterRemoving()
{
	int assignmentsCount = 0;
	int creationsCount = 0;

	if (params.enableClusterRemoving > 0 && this->time % params.enableClusterRemoving == 0 && clusters.size() > 0)
	{
		std::shared_ptr<PathCluster> cluster = clusters.front();
#if REMOVE_CLUSTER_WHEN_FINISHED
		finishedClusters.push_back(cluster);
#endif
		clusters.pop_front();

		std::list<std::shared_ptr<Path>> clusterPaths;
		
		for (auto p = cluster->elements.begin(); p != cluster->elements.end(); ++p)
		{
			auto & path = *p;
			clusterPaths.push_back(path);
			assignments.erase(path->id);
		}

		for (auto p = clusterPaths.begin(); p != clusterPaths.end(); ++p)
		{
			auto & path = *p;

			std::shared_ptr<PathDistance> minDistance;
			std::shared_ptr<PathCluster> closestCluster;
			findClosestCluster(path, closestCluster, minDistance);

			if (closestCluster.get() != NULL) {
				// assign to the closest cluster
				closestCluster->assign(path, *minDistance, params);
				++assignmentsCount;
			}
			else {
				// create a new cluster containing single path and add it to collection
				closestCluster = std::make_shared<PathCluster>(path);
				minDistance = PathDistanceFactory::create(*closestCluster, *path, params);
				clusters.push_back(closestCluster);
				++creationsCount;
			}
			// mkloszczyk: assignments.insert(std::map<unsigned long, std::shared_ptr<PathCluster>>::value_type(path->id, closestCluster));
			assignments.insert(path->id, closestCluster, time, minDistance);
		}

		for (auto p = clusterPaths.begin(); p != clusterPaths.end(); ++p)
		{
			auto & path = *p;
			if (path->removed)
			{
				auto a = assignments.find(path->id);
				if (a != assignments.end())
				{
					// mkloszczyk: auto & cluster = a->second;
					auto & cluster = a->second.cluster;
					if (cluster->markPathEnd(path))
					{
						almostFinishedClusters.push_back(cluster);
						clusters.remove(cluster);
					}
					assignments.erase(a);
				}
			}
		}
	}

	STATS_WRITE("count path removing-migration-assignment", assignmentsCount);
	STATS_WRITE("count path removing-migration-created", creationsCount);

	STATS_ADD("count-total path removing-migration-assignment", assignmentsCount);
	STATS_ADD("count-total path removing-migration-created", creationsCount);
}

/*void motion_analysis::PathClustering::doMultitargetTracking()
{
	if (!params.enableMultitrackingTarget)
		return;

	std::vector<cv::Point2d> pts;
	for (auto c = clusters.begin(); c != clusters.end(); ++c)
	{
		auto& cluster = *c;
		if (cluster->begin() == cluster->end())
			continue;

		auto point = cluster->rbegin();

		if (point->t == this->time)
		{
			cv::Point2d p(point->x, point->y);
			pts.push_back(p);
		}
	}
}*/

/// <summary>
/// See declaration for all the details.
/// </summary>
const ::size_t motion_analysis::PathClustering::computeMemoryUsage()
{
	::size_t mem = 0;
	::size_t aux;

	// paths
	typedef decltype(paths) paths_type;
	aux = sizeof(paths_type::value_type) * paths.size();
	statistics["mem-paths"] = static_cast<StatisticsProvider::ValueType>(aux);
	mem += aux;


	/// clusters
	typedef decltype(clusters) clusters_type; 
	aux = sizeof(clusters_type::value_type) * clusters.size();
	for (auto c = clusters.begin(); c != clusters.end(); ++c) {
		aux += (**c).computeMemoryUsage();
	}
	statistics["mem-clusters"] = static_cast<StatisticsProvider::ValueType>(aux);
	mem += aux;

	/// altered clusters
	typedef decltype(finishedClusters) finishedClusters_type; 
	aux = sizeof(finishedClusters_type::value_type) * finishedClusters.size();
	for (auto c = finishedClusters.begin(); c != finishedClusters.end(); ++c) {
		aux += (**c).computeMemoryUsage();
	}
	statistics["mem-finishedClusters"] = static_cast<StatisticsProvider::ValueType>(aux);
	mem += aux;

	/// altered clusters
	typedef decltype(almostFinishedClusters) almostFinishedClusters_type;
	aux = sizeof(almostFinishedClusters_type::value_type) * almostFinishedClusters.size();
	for (auto c = almostFinishedClusters.begin(); c != almostFinishedClusters.end(); ++c) {
		aux += (**c).computeMemoryUsage();
	}
	statistics["mem-almostFinishedClusters"] = static_cast<StatisticsProvider::ValueType>(aux);
	mem += aux;

	// assignments
	typedef decltype(assignments) assignments_type; 
	aux = sizeof(assignments_type::value_type) * assignments.size();
	statistics["mem-assignments"] = static_cast<StatisticsProvider::ValueType>(aux);
	mem += aux;

	statistics["mem-whole"] = static_cast<StatisticsProvider::ValueType>(mem);

	return mem;
}

/// <summary>
/// See declaration for all the details.
/// </summary>
void motion_analysis::PathClustering::findClosestCluster(const std::shared_ptr<Path>& path, std::shared_ptr<PathCluster>& closestCluster, std::shared_ptr<PathDistance>& minDistance)
{
	for (auto c = clusters.begin(); c != clusters.end(); ++c) {
		auto & cluster = *c;

		auto distance = PathDistanceFactory::create(*cluster, *path, params);

		// check if distance bounds are met
		if (distance->checkConditions(params.pathThresholds)) {
			// update pointers if there is no minimum distance or current distance is smaller than minimum
			if (minDistance.get() == NULL || minDistance->total > distance->total) {
				minDistance = distance;
				closestCluster = cluster;
			}
		}
	}
}

void motion_analysis::PathClustering::processAlmostFinishedClusters()
{
	finishedClusters.insert(finishedClusters.end(), almostFinishedClusters.begin(), almostFinishedClusters.end());
	almostFinishedClusters.clear();
}

/*
void motion_analysis::PathClustering::MovingCluster::doMoving()
{
	pathClustering.lock()->doMovingCluster();
}

void motion_analysis::PathClustering::MovingPath::doMoving()
{
	pathClustering.lock()->doMovingPath();
}
*/

const ::size_t motion_analysis::PathClustering::calculateHash() const
{
	size_t seed = 0;
	for (auto b = finishedClusters.begin(); b != finishedClusters.end(); ++b) {
			boost::hash_combine(seed, (**b).calculateHash() );	
	}

	return seed; 
}

void motion_analysis::PathClustering::saveState(State& state)
{
	std::ostringstream oss(std::ios::binary | std::ios::out);
	boost::archive::binary_oarchive oa(oss);
	oa << paths;
	oa << clusters;
	oa << assignments;
	oa << almostFinishedClusters;
	oa << finishedClusters;
	state.buffer = oss.str();
	state.time = time;
	state.valid = true;
}

void motion_analysis::PathClustering::saveState()
{
	if (!params.enableHistory)
		return;

	if (stateHistory.size() <= time)
		stateHistory.resize(time + 1);

	//if (!stateHistory[time].valid)
	saveState(stateHistory[time]);
}

bool motion_analysis::PathClustering::loadState(const State& state)
{
	if (!state.valid)
		return false;
	std::istringstream iss(state.buffer, std::ios::binary | std::ios::in);
	boost::archive::binary_iarchive ia(iss);
	ia >> paths;
	ia >> clusters;
	ia >> assignments;
	ia >> almostFinishedClusters;
	ia >> finishedClusters;
	time = state.time;
	return true;
}

bool motion_analysis::PathClustering::backInTime(PathTimeType newTime)
{
	if (!params.enableHistory || stateHistory.size() <= newTime || !loadState(stateHistory[newTime]))
		return false;
	//update();
	return true;
}
