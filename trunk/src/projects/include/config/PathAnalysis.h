#pragma once
#ifndef Config_PathAnalysis_h__
#define Config_PathAnalysis_h__

#include <boost/serialization/access.hpp>

namespace config
{
	class PathAnalysis
	{
	public:
		class DistanceThresholds
		{
		public:
			DistanceThresholds() 
				: m_Total(100), m_Positional(1), m_Tangent(100) { }
			DistanceThresholds(float total, float positional, float tangent) 
				: m_Total(total), m_Positional(positional), m_Tangent(tangent) { }

			float getTotal() const { return m_Total; }
			void setTotal(float val) { m_Total = val; }

			float getPositional() const { return m_Positional; }
			void setPositional(float val) { m_Positional = val; }

			float getTangent() const { return m_Tangent; }
			void setTangent(float val) { m_Tangent = val; }
		private:
			/// Total distance.
			float m_Total;

			/// Positional component of distance.
			float m_Positional;

			/// Tangent component of distance.
			float m_Tangent;

			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version)
			{
				ar & m_Total;
				ar & m_Positional;
				ar & m_Tangent;
			}
		};

		class MergingThresholds
		{
		public:
			MergingThresholds() 
				: m_Time(10), m_X(20.0f), m_Y(20.0f) { }
			MergingThresholds(int time, float x, float y) 
				: m_Time(time), m_X(x), m_Y(y) { }

			int getTime() const { return m_Time; }
			void setTime(int val) { m_Time = val; }

			float getX() const { return m_X; }
			void setX(float val) { m_X = val; }

			float getY() const { return m_Y; }
			void setY(float val) { m_Y = val; }

		private:
			int m_Time;
			float m_X;
			float m_Y;

			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version)
			{
				ar & m_Time;
				ar & m_X;
				ar & m_Y;
			}
		};

		PathAnalysis();

		bool load();
		bool save() const;

		bool isUseOpenMP() const { return m_UseOpenMP; }
		void setUseOpenMP(bool val) { m_UseOpenMP = val; }

		float getAlpha() const { return m_Alpha; }
		void setAlpha(float val) { m_Alpha = val; }

		int getMinimumPathLength() const { return m_MinimumPathLength; }
		void setMinimumPathLength(int val) { m_MinimumPathLength = val; }

		const config::PathAnalysis::DistanceThresholds& getPathThresholds() const { return m_PathThresholds; }
		config::PathAnalysis::DistanceThresholds& getPathThresholds() { return m_PathThresholds; }

		const config::PathAnalysis::DistanceThresholds& getClusterThresholds() const { return m_ClusterThresholds; }
		config::PathAnalysis::DistanceThresholds& getClusterThresholds() { return m_ClusterThresholds; }

		float getWeightY() const { return m_WeightY; }
		void setWeightY(float val) { m_WeightY = val; }

		float getWeightX() const { return m_WeightX; }
		void setWeightX(float val) { m_WeightX = val; }

		float getWeightTau() const { return m_WeightTau; }
		void setWeightTau(float val) { m_WeightTau = val; }

		int getEnableMerging() const { return m_EnableMerging; }
		void setEnableMerging(int val) { m_EnableMerging = val; }

		float getPathsPercentageMovesPerFrame() const { return m_PathsPercentageMovesPerFrame; }
		void setPathsPercentageMovesPerFrame(float val) { m_PathsPercentageMovesPerFrame = val; }

		int getMinPathsMovesPerFrame() const { return m_MinPathsMovesPerFrame; }
		void setMinPathsMovesPerFrame(int val) { m_MinPathsMovesPerFrame = val; }

		bool isCreateNewCluster() const { return m_CreateNewCluster; }
		void setCreateNewCluster(bool val) { m_CreateNewCluster = val; }

		bool isTryMovingOnlyWhenCurrentIsTooFar() const { return m_TryMovingOnlyWhenCurrentIsTooFar; }
		void setTryMovingOnlyWhenCurrentIsTooFar(bool val) { m_TryMovingOnlyWhenCurrentIsTooFar = val; }

		int getEnableClusterRemoving() const { return m_EnableClusterRemoving; }
		void setEnableClusterRemoving(int val) { m_EnableClusterRemoving = val; }

		int getEnableMovingPaths() const { return m_EnableMovingPaths; }
		void setEnableMovingPaths(int val) { m_EnableMovingPaths = val; }

		int getEnableMergingGaps() const { return m_EnableMergingGaps; }
		void setEnableMergingGaps(int val) { m_EnableMergingGaps = val; }

		const config::PathAnalysis::MergingThresholds& getMergingThresholds() const { return m_MergingThresholds; }
		config::PathAnalysis::MergingThresholds& getMergingThresholds() { return m_MergingThresholds; }

		int getMergingOldestClusterTime() const { return m_MergingOldestClusterTime; }
		void setMergingOldestClusterTime(int val) { m_MergingOldestClusterTime = val; }

	private:
		bool m_UseOpenMP;		

		/// Alpha parameter used for calculation of tangential coordinates.
		float m_Alpha;

		/// Minimum length of path to be clustered.
		int m_MinimumPathLength;

		/// Distance thresholds for path assignment.
		DistanceThresholds m_PathThresholds;

		/// Distance thresholds for path merging.
		DistanceThresholds m_ClusterThresholds;

		/// Weight factor in X direction.
		float m_WeightX;

		/// Weight factor in Y direction.
		float m_WeightY;

		/// Tangential weight factor.
		float m_WeightTau;

		/// Flag indicating whether cluster merging should be performed.
		int m_EnableMerging;

		float m_PathsPercentageMovesPerFrame;
		int m_MinPathsMovesPerFrame;

		/// Flag indicating whether new cluster is created when moving and any other cluster doesn't met distance thresholds.
		bool m_CreateNewCluster;

		/// Tries moving only when distance to current cluster doesn't met distance thresholds.
		bool m_TryMovingOnlyWhenCurrentIsTooFar;

		/// Flag indicating whether cluster removing (to change paths clusters) should be performed.
		int m_EnableClusterRemoving;

		/// Flag indicating whether path migration should be performed.
		int m_EnableMovingPaths;

		int m_EnableMergingGaps;
		MergingThresholds m_MergingThresholds;
		int m_MergingOldestClusterTime;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};

	template<class Archive>
	void config::PathAnalysis::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_UseOpenMP;
		ar & m_Alpha;
		ar & m_MinimumPathLength;
		ar & m_PathThresholds;
		ar & m_ClusterThresholds;
		ar & m_WeightX;
		ar & m_WeightY;
		ar & m_WeightTau;
		ar & m_EnableMerging;

		ar & m_PathsPercentageMovesPerFrame;
		ar & m_MinPathsMovesPerFrame;

		ar & m_CreateNewCluster;
		ar & m_TryMovingOnlyWhenCurrentIsTooFar;
		ar & m_EnableClusterRemoving;
		ar & m_EnableMovingPaths;

		ar & m_EnableMergingGaps;
		ar & m_MergingThresholds;
		ar & m_MergingOldestClusterTime;
	}

}

#endif // Config_PathAnalysis_h__