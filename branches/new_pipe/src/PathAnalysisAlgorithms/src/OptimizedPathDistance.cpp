#include <cmath>

#include "PathAnalysisAlgorithms/algorithms/OptimizedPathDistance.h"
#include "PathAnalysisAlgorithms/algorithms/PathCluster.h"
#include "PathAnalysisAlgorithms/algorithms/auxmath.h"

using namespace motion_analysis;

/// <summary>
/// See declaration for all the details.
/// </summary>
motion_analysis::OptimizedPathDistance::OptimizedPathDistance(
	const motion_analysis::PathCluster& cluster,
	const motion_analysis::Path& path,
	const PathClusteringParams& params) : PathDistance()
{
	assert(cluster.points.size() > 1);
	assert(path.points.size() > 1);

	if (cluster.category != path.category) {
		this->setToMax();
		return;
	}

	PathPositionType dt_numerator = 0;
	PathPositionType dt_denominator_P = 0;
	PathPositionType dt_denominator_Z = 0;

	// determine overlap starting from last path and cluster points
	auto &Z = cluster.points;
	auto &P = path.points;

	auto z = Z.rbegin();
	auto p = P.rbegin();
	auto z2 = Z.begin();
	auto p2 = P.begin();

	// check if path overlaps cluster for any time coordinate.
	if (z2->t < p->t && p2->t < z->t)
	{
		// calculate threshold for squaredDispertion
		float maxLength = (std::min(z->t, p->t) - std::max(z2->t, p2->t)) * params.pathThresholds.positional;
		float squaredDispertionThresholdX = maxLength * sqr(params.weightX);
		float squaredDispertionThresholdY = maxLength * sqr(params.weightY);

		for (; z != Z.rend() && p != P.rend(); ++z) {
			while (p != P.rend() && p->t > z->t) {
				++p; // decrement path point until its time coordinate is less or equal than current cluster point
			}

			if (p != P.rend() && (p->t == z->t)) { // if there is an overlap at current point
				// update d components
				squaredDispertion.x += sqr(p->x - z->x - cluster.b.x); // eq. 7.x
				squaredDispertion.y += sqr(p->y - z->y - cluster.b.y); // eq. 7.y

				if (squaredDispertion.x >= squaredDispertionThresholdX || squaredDispertion.y >= squaredDispertionThresholdY)
				{
					// more calculations hasn't got sense, checkCalibration will return false for so big squaredDispertion.
					this->setToMax();
					return;
				}

				// update dt components
				dt_numerator += sqr(p->tau_x - z->tau_x) + sqr(p->tau_y - z->tau_y); // eq. 8 numerator
				dt_denominator_P += sqr(p->tau_x) + sqr(p->tau_y); // eq. 8 denominator left
				dt_denominator_Z += sqr(z->tau_x) + sqr(z->tau_y); // eq. 8 denominator right

				// update beta
				dispertion.x += p->x - z->x; // eq 15. inside brackets
				dispertion.y += p->y - z->y; // eq 16. inside brackets

				++length;
			}
		}
	}

	if (length > 0) {
		// positional components are analyzed separately now
		squaredDispertion.x /= length * sqr(params.weightX);	// eq. 7.x normalization
		squaredDispertion.y /= length * sqr(params.weightY);	// eq. 7.y normalization
		tangentialComponent = dt_numerator / (std::min(dt_denominator_P, dt_denominator_Z) + (length * 2));	// eq. 8 division
		dispertion *= 1.0 / length;			// eq 15. and 16. outside brackets

		assert(tangentialComponent < 10e10);

		cv::Point_<PathPositionType> diff;
		diff.x = sqr(dispertion.x - cluster.b.x); // eq. 17
		diff.y = sqr(dispertion.y - cluster.b.y); // eq. 18

		// new values of mean displacement
		float bx = (cluster.b.x * cluster.bk + dispertion.x) / (cluster.bk + 1); // eq. 23 (path.c.x is equal to dispertion.x)
		float by = (cluster.b.y * cluster.bk + dispertion.y) / (cluster.bk + 1); // eq. 24 (path.c.y is equal to dispertion.y)

		PathPositionType x = std::max(bx, diff.x) / sqr(params.weightX);	// eq 14.x
		PathPositionType y = std::max(by, diff.y) / sqr(params.weightY);	// eq 14.y
		PathPositionType t = tangentialComponent /= sqr(params.weightTau);	// eq 14.t

		total = x + y + t;

	//	std::cout << "( " << total << ", " << squaredDispertion.x << ", " << squaredDispertion.y << ", " << tangentialComponent << "), "; 

	}
	else {
		this->setToMax();
	}
}

/// <summary>
/// See declaration for all the details.
/// </summary>
motion_analysis::OptimizedPathDistance::OptimizedPathDistance(
	const motion_analysis::PathCluster& cluster1,
	const motion_analysis::PathCluster& cluster2,
	const PathClusteringParams& params) : PathDistance()
{
	assert(cluster1.points.size() > 1);
	assert(cluster2.points.size() > 1);

	PathPositionType dt_numerator = 0;
	PathPositionType dt_denominator_A = 0;
	PathPositionType dt_denominator_B = 0;
	cv::Point_<PathPositionType> deviation = cluster1.b - cluster2.b;

	// determine overlap starting from last path and cluster points
	auto &A = cluster1.points;
	auto &B = cluster2.points;

	auto a = A.rbegin();
	auto b = B.rbegin();
	auto a2 = A.begin();
	auto b2 = B.begin();

	// check if both clusters overlap each other for any time coordinate.
	if (a2->t <= b->t && b2->t <= a->t)
	{
		// calculate threshold for squaredDispertion
		float maxLength = std::max(1, (std::min(a->t, b->t) - std::max(a2->t, b2->t))) * params.clusterThresholds.positional;
		float squaredDispertionThresholdX = maxLength * sqr(params.weightX);
		float squaredDispertionThresholdY = maxLength * sqr(params.weightY);

		for (; b != B.rend() && a != A.rend(); ++b) {
			while (a != A.rend() && a->t > b->t) {
				++a; // decrement A point until its time coordinate is less or equal than B point
			}

			if (a != A.rend() && (a->t == b->t)) { // if there is an overlap at current point
				// update d components
				squaredDispertion.x += sqr(a->x - b->x + deviation.x); // eq. 34.x
				squaredDispertion.y += sqr(a->y - b->y + deviation.y); // eq. 34.y

				if (squaredDispertion.x >= squaredDispertionThresholdX || squaredDispertion.y >= squaredDispertionThresholdY)
				{
					// more calculations hasn't got sense, checkCalibration will return false for so big squaredDispertion.
					this->setToMax();
					return;
				}

				// update dt components
				dt_numerator += sqr(a->tau_x - b->tau_x) + sqr(a->tau_y - b->tau_y); // eq. 35 numerator
				dt_denominator_A += sqr(a->tau_x) + sqr(a->tau_y); // eq. 35 denominator left
				dt_denominator_B += sqr(b->tau_x) + sqr(b->tau_y); // eq. 35 denominator right

				// update beta
				dispertion.x += a->x - b->x; // eq. 37. inside brackets without deviation
				dispertion.y += a->y - b->y; // eq. 36. inside brackets without deviation

				++length;
			}
		}
	}

	if (length > 0) {
		// positional components are analyzed separately now
		squaredDispertion.x /= length * sqr(params.weightX);	// eq. 34.x normalization
		squaredDispertion.y /= length * sqr(params.weightY);	// eq. 34.y normalization
		tangentialComponent = dt_numerator / (std::min(dt_denominator_A, dt_denominator_B) + (length * 2));	// eq. 35 division
		dispertion *= 1.0 / length;			// eq. 37 and 38 length normalization

		assert(tangentialComponent < 10e10);

		cv::Point_<PathPositionType> diff;
		diff.x = sqr(dispertion.x + cluster1.b.x - cluster2.b.x); // eq. 37
		diff.y = sqr(dispertion.y + cluster1.b.y - cluster2.b.y); // eq. 38

		// diff is always >=0 so std::max(0.0f, diff.x) can be omitted.
		PathPositionType x = diff.x / sqr(params.weightX); 	// eq. 36
		PathPositionType y = diff.y / sqr(params.weightY);
		PathPositionType t = tangentialComponent / sqr(params.weightTau);
		total = x + y + t;

	}
	else {
		this->setToMax();
	}
}
