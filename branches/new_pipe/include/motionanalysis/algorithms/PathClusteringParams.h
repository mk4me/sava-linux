#pragma once
#include <memory>
#include <map>
#include <string>
#include "ParamSet.h"

namespace motion_analysis {

    /// <summary>
    /// Gathers distance thresholds.
    /// </summary>
    class DistanceThresholds
    {
        public:
        /// Total distance.
        float total;

        /// Positional component of distance.
        float positional;

        /// Tangent component of distance.
        float tangent;

		
	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & SERIALIZATION_NVP(total);
			ar & SERIALIZATION_NVP(positional);
			ar & SERIALIZATION_NVP(tangent);
		}
    };

	class MergingThresholds
	{
	public:
		int time;
		float x;
		float y;

		MergingThresholds();

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & SERIALIZATION_NVP(time);
			ar & SERIALIZATION_NVP(x);
			ar & SERIALIZATION_NVP(y);
		}
	};


    /// <summary>
    /// Set of clustering parameters.
    /// </summary>
    class PathClusteringParams : public ParamSet
    {
        public:
        /// Alpha parameter used for calculation of tangential coordinates.
        float alpha;

        /// Minimum length of path to be clustered.
        int minimumPathLength;

        /// Distance thresholds for path assignment.
        DistanceThresholds pathThresholds;

        /// Distance thresholds for path merging.
        DistanceThresholds clusterThresholds;

        /// Weight factor in X direction.
        float weightX;

        /// Weight factor in Y direction.
        float weightY;

        /// Tangential weight factor.
        float weightTau;

		/// Flag indicating whether cluster merging should be performed.
		int enableMerging;

		float pathsPercentageMovesPerFrame;

		int minPathsMovesPerFrame;

		/// Flag indicating whether new cluster is created when moving and any other cluster doesn't met distance thresholds.
		bool createNewCluster;

		/// Tries moving only when distance to current cluster doesn't met distance thresholds.
		bool tryMovingOnlyWhenCurrentIsToFar;

		/// Flag indicating whether cluster removing (to change paths clusters) should be performed.
		int enableClusterRemoving;

		/// Flag indicating whether path migration should be performed.
		int enableMovingPaths;
		
		int enableMergingGaps;
		MergingThresholds mergingThresholds;
		int mergingOldestClusterTime;

		//bool enableMultitrackingTarget;
		//bool enableClusterSplitting;

		//int domPointsMin;
		//int domPointsMax;
		//int domPointsNeigh;
		//int domPointsMaxAngle;

		bool enableHistory;

        /// Initializes wrappers for parameters.
        PathClusteringParams();
	
	private:
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & SERIALIZATION_BASE_OBJECT_NVP(ParamSet);

			ar & SERIALIZATION_NVP(alpha);
			ar & SERIALIZATION_NVP(minimumPathLength);
			ar & SERIALIZATION_NVP(pathThresholds);
			ar & SERIALIZATION_NVP(clusterThresholds);
			ar & SERIALIZATION_NVP(weightX);
			ar & SERIALIZATION_NVP(weightY);
			ar & SERIALIZATION_NVP(weightTau);
			ar & SERIALIZATION_NVP(enableMerging);
			ar & SERIALIZATION_NVP(pathsPercentageMovesPerFrame);
			ar & SERIALIZATION_NVP(minPathsMovesPerFrame);

			ar & SERIALIZATION_NVP(enableMergingGaps);
			ar & SERIALIZATION_NVP(mergingThresholds);
			ar & SERIALIZATION_NVP(mergingOldestClusterTime);
		}
    };

}

BOOST_CLASS_EXPORT_KEY(motion_analysis::PathClusteringParams);