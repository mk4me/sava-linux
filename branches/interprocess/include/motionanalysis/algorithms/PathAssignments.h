#pragma once

#include <memory>
#include <map>
#include "Point.h"

namespace motion_analysis 
{
	class PathCluster;
	class PathDistance;

	class PathAssignments
	{
	public:
		typedef uint64_t PathId;
		struct AssignmentData
		{
		public:
			AssignmentData() {}
			AssignmentData(const std::shared_ptr<PathCluster>& cluster, PathTimeType time, const std::shared_ptr<PathDistance>& distance)
				: cluster(cluster), time(time), distance(distance)
			{ }
			std::shared_ptr<PathCluster> cluster;
			PathTimeType time;
			std::shared_ptr<PathDistance> distance;

		private:
			friend class boost::serialization::access;

			template<class Archive>
			void serialize(Archive & ar, const unsigned int version)
			{
				ar & SERIALIZATION_NVP(cluster);
				ar & SERIALIZATION_NVP(time);
				ar & SERIALIZATION_NVP(distance);
			}
		};

	private:
		typedef std::map<PathId, AssignmentData> Assignments;

	public:
		typedef Assignments::iterator iterator;
		typedef Assignments::const_iterator const_iterator;
		typedef Assignments::key_type key_type;
		typedef Assignments::value_type value_type;
		typedef Assignments::size_type size_type;

		PathAssignments();
		~PathAssignments();

		iterator begin() { return assignments.begin(); }
		const_iterator begin() const { return assignments.begin(); }

		iterator end() { return assignments.end(); }
		const_iterator end() const { return assignments.end(); }

		iterator find(const key_type& key) { return assignments.find(key); }
		const_iterator find(const key_type& key) const { return assignments.find(key); }

		size_type erase(const key_type& key) { return assignments.erase(key); }
		void erase(const_iterator it) { assignments.erase(it); }

		//void insert(PathId id, const std::shared_ptr<PathCluster>& cluster, PathTimeType time);
		void insert(PathId id, const std::shared_ptr<PathCluster>& cluster, PathTimeType time, const std::shared_ptr<PathDistance>& distance)
		{
			assignments.insert(value_type(id, AssignmentData(cluster, time, distance)));
		}
		//void change(PathId id, const std::shared_ptr<PathCluster>& cluster, PathTimeType time);
		void change(PathId id, const std::shared_ptr<PathCluster>& cluster, PathTimeType time, const std::shared_ptr<PathDistance>& distance)
		{
			assignments[id] = AssignmentData(cluster, time, distance);
		}

		void reset() { Assignments().swap(assignments); }

		const std::shared_ptr<PathCluster>& getCluster(const key_type& key) { return assignments[key].cluster; }
		const std::shared_ptr<PathCluster>& getCluster(iterator it) { return it->second.cluster; }

		PathTimeType getTime(const key_type& key) { return assignments[key].time; }
		PathTimeType getTime(iterator it) { return it->second.time; }

		const std::shared_ptr<PathDistance>& getDistance(const key_type& key) { return assignments[key].distance; }
		const std::shared_ptr<PathDistance>& getDistance(iterator it) { return it->second.distance; }

		size_type size() const { return assignments.size(); }

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & SERIALIZATION_NVP(assignments);
		}
		Assignments assignments;
	};
}

BOOST_CLASS_EXPORT_KEY(motion_analysis::PathAssignments);
BOOST_CLASS_TRACKING(motion_analysis::PathAssignments, boost::serialization::track_selectively);