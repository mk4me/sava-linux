#include "motionanalysis/algorithms/PathEstimator.h"

#include "opencv2/calib3d.hpp"
#include <motionanalysis/algorithms/Point.h>
#include "opencv2/imgproc.hpp"

namespace motion_analysis
{
	int PathEstimator::BOUNDING_BOX_PARAM_R = 6;
	int PathEstimator::BOUNDING_BOX_PARAM_T = 8;
	int PathEstimator::BOUNDING_BOX_DEFAULT_FRAMES_COUNT = 75;

	float PathEstimator::CENTER_PARAM_THRESHOLD = 36;
	float PathEstimator::CENTER_PARAM_W = 0.1f;

	PathEstimator::PathEstimator(const cv::Mat1d& t /*= cv::Mat1d::eye(3,3)*/) : m_Transform(t)
	{
	}

	/* ---------------------------------------------------------- */
	void PathEstimator::update(const std::list<std::shared_ptr<PathCluster>> &clusters, PathTimeType time)
	{
		for (auto& cluster : clusters)
		{
			//next estimated point (must be calculated)
			Point nextcenterPoint;

			//estimated method
			EstimatedMethod estimatedMethod = FROM_NO_PAIR;

			if (!cluster->centers.empty())
			{
				//prev estimated point
				auto& prevEstimatedPoint = cluster->centers.back();

				//points from current and previous paths
				std::vector<cv::Point3f> currPoints;
				std::vector<cv::Point3f> prevPoints;

				for (auto& path : cluster->elements) {
					if (!path->removed && path->points.size() > 1) {
						auto currIt = std::prev(path->points.end());
						auto prevIt = std::prev(currIt);

						if (currIt != prevIt) {
							currPoints.push_back(getTransformed(cv::Point3f(currIt->x, currIt->y, 0)));
							prevPoints.push_back(getTransformed(cv::Point3f(prevIt->x, prevIt->y, 0)));
						}
					}
				}

				//calc next estimated points
				size_t pairNum = currPoints.size();
				if (pairNum == 0)
				{
					estimatedMethod = FROM_NO_PAIR;
				}
				else if (pairNum == 1)
				{
					calcFromOnePair(prevPoints, currPoints, prevEstimatedPoint, nextcenterPoint);
					estimatedMethod = FROM_ONE_PAIR;
				}
				else if (pairNum == 2)
				{
					calcFromTwoPairs(prevPoints, currPoints, prevEstimatedPoint, nextcenterPoint);
					estimatedMethod = FROM_TWO_PAIRS;
				}
				else
				{
					//if (calcFromThreePairs_1(prevPoints, currPoints, prevEstimatedPoint, nextEstimatedPoint))
					//{
					//	estimatedMethod = FROM_THREE_PAIRS;
					//}
					//else
					//{
					calcFromThreePairs_2(prevPoints, currPoints, prevEstimatedPoint, nextcenterPoint);
					estimatedMethod = FROM_THREE_PAIRS_ALTERNATIVE;
					//}
				}
			}


			if (estimatedMethod == FROM_NO_PAIR)
				nextcenterPoint = getTransformed(*std::prev(cluster->end()));

			//wyeliminuj uciekanie punktu od klastra
			escapeElimination(cluster, nextcenterPoint);

			//push next estimated point to cluster
			nextcenterPoint.t = time;
			cluster->centers.push_back(nextcenterPoint);
		}
	}


	/* ---------------------------------------------------------- */
	void PathEstimator::drawPoint(const std::shared_ptr<PathCluster>& i_Cluster, cv::Mat& i_Image, const cv::Scalar& i_Color) const
	{
		for (auto& estimatorIt : i_Cluster->centers)
		{
			cv::Point2f p(estimatorIt.x, estimatorIt.y);
			cv::circle(i_Image, p, 3, i_Color, 3);

			cv::Scalar innerColor = cv::Scalar(0, 0, 0, 255); //black
			cv::circle(i_Image, p, 3, innerColor, 2);
		}
	}

	/* ---------------------------------------------------------- */
	void PathEstimator::drawBoundingBox(const std::shared_ptr<PathCluster>& i_Cluster, cv::Mat& i_Image, PathTimeType i_Time)
	{
		cv::Rect rect = getBoundingBox(i_Cluster, i_Time);
		if (rect.width != 0 && rect.height != 0)
			cv::rectangle(i_Image, rect, cv::Scalar(0, 0, 255, 255), 1);
	}

	/* ---------------------------------------------------------- */
	cv::Rect PathEstimator::getBoundingBox(const std::shared_ptr<PathCluster>& cluster, PathTimeType endTime, size_t frameCount /*= 0*/) const
	{
		if (frameCount == 0)
			frameCount = BOUNDING_BOX_DEFAULT_FRAMES_COUNT;


		PathTimeType startTime = endTime - static_cast<PathTimeType>(frameCount)+1;

		if (startTime < 0)
			startTime = 0;

		if (cluster->centers.empty())
			return cv::Rect();

		if (cluster->centers.begin()->t < startTime && cluster->centers.rbegin()->t > endTime)
			return cv::Rect();

		std::map<PathTimeType, std::pair<motion_analysis::Point, std::vector<motion_analysis::Point>>> _map;

		for (auto& p : cluster->centers)
			if (p.t >= startTime && p.t <= endTime)
				_map[p.t] = std::make_pair(p, std::vector<motion_analysis::Point>());


		//collect path points
		for (auto& path : cluster->elements)
			for (auto it = path->begin(); it != path->end(); ++it)
				if (it->t >= startTime && it->t <= endTime) {
					auto mapIt = _map.find(it->t);
					if (mapIt != _map.end())
						mapIt->second.second.push_back(getTransformed(*it));
				}


		//calc left, right, up, down vectors
		std::vector<float> left, right, up, down;
		for (auto& mapElem : _map)
		{
			if (!mapElem.second.second.empty())
			{
				auto centerPoint = mapElem.second.first;
				bool isInitialized = false;

				for (auto& pathPoint : mapElem.second.second)
				{
					float l = centerPoint.x - pathPoint.x;
					float r = pathPoint.x - centerPoint.x;
					float u = centerPoint.y - pathPoint.y;
					float d = pathPoint.y - centerPoint.y;

					if (l == 0 && r == 0 && u == 0 && d == 0)
						continue;

					if (!isInitialized)
					{
						left.push_back(l);
						right.push_back(r);
						up.push_back(u);
						down.push_back(d);

						isInitialized = true;
					}
					else
					{
						*left.rbegin() = std::max(*left.rbegin(), l);
						*right.rbegin() = std::max(*right.rbegin(), r);
						*up.rbegin() = std::max(*up.rbegin(), u);
						*down.rbegin() = std::max(*down.rbegin(), d);
					}
				}
			}
		}

		if (left.empty())
			return cv::Rect();

		auto func = [=](std::vector<float>& v)->float {

			std::sort(v.rbegin(), v.rend());

			int L = static_cast<int>(v.size()) - 1;

			int k = 0;
			for (;; k++) {
				if (k + BOUNDING_BOX_PARAM_R > L) break;
				if ((v[k] - v[k + BOUNDING_BOX_PARAM_R]) < BOUNDING_BOX_PARAM_T) break;
			}

			return v[k];
		};


		float leftR = func(left);
		float rightR = func(right);
		float upR = func(up);
		float downR = func(down);

		auto centerPoint = cluster->centers.back();

		cv::Rect2f bb;
		bb.x = centerPoint.x - leftR;
		bb.y = centerPoint.y - upR;
		bb.width = rightR + leftR;
		bb.height = upR + downR;

		return bb;
	}

	/* ---------------------------------------------------------- */
	PathEstimator::BoxesMap PathEstimator::getBoundingBoxes(const std::shared_ptr<PathCluster>& cluster, PathTimeType endTime, size_t frameCount /*= 0*/)
	{
		if (frameCount == 0)
			frameCount = BOUNDING_BOX_DEFAULT_FRAMES_COUNT;

		PathTimeType startTime = endTime - static_cast<PathTimeType>(frameCount)+1;

		if (startTime < 0)
			startTime = 0;

		if (cluster->centers.empty())
			return BoxesMap();

		if (cluster->centers.begin()->t < startTime && cluster->centers.rbegin()->t > endTime)
			return BoxesMap();

		std::map<PathTimeType, std::pair<motion_analysis::Point, std::vector<motion_analysis::Point>>> _map;

		for (auto& p : cluster->centers)
			if (p.t >= startTime && p.t <= endTime)
				_map[p.t] = std::make_pair(p, std::vector<motion_analysis::Point>());


		//collect path points
		for (auto& path : cluster->elements)
			for (auto it = path->begin(); it != path->end(); ++it)
				if (it->t >= startTime && it->t <= endTime) {
					auto mapIt = _map.find(it->t);
					if (mapIt != _map.end())
						mapIt->second.second.push_back(getTransformed(*it));
				}


		//calc left, right, up, down vectors
		std::vector<float> left, right, up, down;
		for (auto& mapElem : _map)
		{
			if (!mapElem.second.second.empty())
			{
				auto centerPoint = mapElem.second.first;
				bool isInitialized = false;

				for (auto& pathPoint : mapElem.second.second)
				{
					float l = centerPoint.x - pathPoint.x;
					float r = pathPoint.x - centerPoint.x;
					float u = centerPoint.y - pathPoint.y;
					float d = pathPoint.y - centerPoint.y;

					if (l == 0 && r == 0 && u == 0 && d == 0)
						continue;

					if (!isInitialized)
					{
						left.push_back(l);
						right.push_back(r);
						up.push_back(u);
						down.push_back(d);

						isInitialized = true;
					}
					else
					{
						*left.rbegin() = std::max(*left.rbegin(), l);
						*right.rbegin() = std::max(*right.rbegin(), r);
						*up.rbegin() = std::max(*up.rbegin(), u);
						*down.rbegin() = std::max(*down.rbegin(), d);
					}
				}
			}
		}

		if (left.empty())
			return BoxesMap();

		auto func = [=](std::vector<float>& v)->float {

			std::sort(v.rbegin(), v.rend());

			int L = static_cast<int>(v.size()) - 1;

			int k = 0;
			for (;; k++) {
				if (k + BOUNDING_BOX_PARAM_R > L) break;
				if ((v[k] - v[k + BOUNDING_BOX_PARAM_R]) < BOUNDING_BOX_PARAM_T) break;
			}

			return v[k];
		};


		float leftR = func(left);
		float rightR = func(right);
		float upR = func(up);
		float downR = func(down);


		BoxesMap boxes;

		//calc box positions for cluster in each frame 
		for (PathTimeType i = startTime; i <= endTime; i++)
		{
			auto mapIt = _map.find(i);
			if (mapIt != _map.end())
			{
				Point& centerPoint = mapIt->second.first;

				cv::Rect2f box;
				box.x = centerPoint.x - leftR;
				box.y = centerPoint.y - upR;
				box.width = rightR + leftR;
				box.height = upR + downR;

				boxes.insert(std::make_pair(i, box));
			}
		}

		return boxes;
	}

	/* ---------------------------------------------------------- */
	bool PathEstimator::calcFromOnePair(const std::vector<cv::Point3f>& i_Points_1, const std::vector<cv::Point3f>& i_Points_2, const Point& i_PrevPoint, Point& o_NextPoint) const
	{
		assert(i_Points_1.size() == 1 && i_Points_2.size() == 1);

		//There is only one pair of corresponding paths : 
		//The last point of the path in frame n is P1, and the last point of the path in frame n + 1 is Q1.
		//The translation vector  is   V = Q1 - P1

		cv::Point3f translate = getAverageTranslation(i_Points_1, i_Points_2);
		o_NextPoint.x = i_PrevPoint.x + translate.x;
		o_NextPoint.y = i_PrevPoint.y + translate.y;

		return true;
	}

	/* ---------------------------------------------------------- */
	bool PathEstimator::calcFromTwoPairs(const std::vector<cv::Point3f>& i_Points_1, const std::vector<cv::Point3f>& i_Points_2, const Point& i_PrevPoint, Point& o_NextPoint) const
	{
		assert(i_Points_1.size() == 2 && i_Points_2.size() == 2);

		//There are two pairs of corresponding paths.
		//Their last points are P1, Q1 and P2, Q2:   
		//We take the average tranlation vector : V = ((Q1 - P1) + (Q2 - P2)) / 2

		cv::Point3f translate = getAverageTranslation(i_Points_1, i_Points_2);
		o_NextPoint.x = i_PrevPoint.x + translate.x;
		o_NextPoint.y = i_PrevPoint.y + translate.y;

		return true;
	}

	/* ---------------------------------------------------------- */
	bool PathEstimator::calcFromThreePairs_1(
		const std::vector<cv::Point3f>& i_Points_1,
		const std::vector<cv::Point3f>& i_Points_2,
		const Point& i_PrevPoint, Point& o_NextPoint) const
	{
		assert(i_Points_1.size() > 2 && i_Points_2.size() > 2 && i_Points_1.size() == i_Points_2.size());

		std::vector<uchar> inliers;
		cv::Mat mat3x4(3, 4, CV_64F);

		int ret = estimateAffine3D(i_Points_1, i_Points_2, mat3x4, inliers, 1, 0.999);
		if (ret)
		{
			cv::Mat mat2x3(2, 3, CV_64F);
			mat2x3.at<double>(0, 0) = mat3x4.at<double>(0, 0);
			mat2x3.at<double>(0, 1) = mat3x4.at<double>(0, 1);
			mat2x3.at<double>(0, 2) = mat3x4.at<double>(0, 3);
			mat2x3.at<double>(1, 0) = mat3x4.at<double>(1, 0);
			mat2x3.at<double>(1, 1) = mat3x4.at<double>(1, 1);
			mat2x3.at<double>(1, 2) = mat3x4.at<double>(1, 3);

			o_NextPoint.x = static_cast<float>(i_PrevPoint.x * mat2x3.at<double>(0, 0) + i_PrevPoint.y * mat2x3.at<double>(0, 1) + mat2x3.at<double>(0, 2));
			o_NextPoint.y = static_cast<float>(i_PrevPoint.x * mat2x3.at<double>(1, 0) + i_PrevPoint.y * mat2x3.at<double>(1, 1) + mat2x3.at<double>(1, 2));
		}

		return ret;
	}


	/* ---------------------------------------------------------- */
	bool PathEstimator::calcFromThreePairs_2(const std::vector<cv::Point3f>& i_Points_1, const std::vector<cv::Point3f>& i_Points_2, const Point& i_PrevPoint, Point& o_NextPoint) const
	{
		assert(i_Points_1.size() > 2 && i_Points_2.size() > 2 && i_Points_1.size() == i_Points_2.size());


		std::vector<cv::Point3f> p = i_Points_1;
		std::vector<cv::Point3f> q = i_Points_2;


		cv::Point3f v;
		while (true)
		{
			//compute the average translation vector V from all the pairs of corresponding point
			v = getAverageTranslation(p, q);

			//if there only two pairs of corresponding points left in the reduced set, then break the loop
			if (p.size() == 2)
				break; //breaking 'while' loop

			//for each pair of the corresponding points P and Q calculate the error  vector : E = Q - (P + V), and the squared length of the error vector D
			std::vector<float> d(p.size());
			for (int i = 0; i < d.size(); i++)
			{
				cv::Point3f e = q[i] - (p[i] + v);
				d[i] = (e.x*e.x) + (e.y*e.y);
			}

			//find the pair of points with the largest D.
			auto it = std::max_element(d.begin(), d.end());
			if (*it <= CENTER_PARAM_THRESHOLD)
				break; //breaking 'while' loop

			//auto it = std::find_if(d.begin(), d.end(), [&](float val) { return val > THRESHOLD; });
			//if (it == d.end())
			//	break; //breaking 'while' loop

			//remove this pair from and recompute a new average translation vector v (in 'while' loop)
			size_t index = std::distance(d.begin(), it);
			p.erase(p.begin() + index);
			q.erase(q.begin() + index);
		}

		o_NextPoint.x = i_PrevPoint.x + v.x;
		o_NextPoint.y = i_PrevPoint.y + v.y;

		return true;
	}

	void PathEstimator::escapeElimination(const std::shared_ptr<PathCluster>& i_Cluster, Point& i_PointToCorrent) const
	{

		//Uaktualnianie punktu centrum

		//	C - pozycja centrum klastra w klatce n(po transformacji z klatki n - 1
		//	P - ostateczna pozycja klastra w klatce n, po wszystkich aktualizacjach
		//	C' - pozycja centrum po aktualizacji wzgledem pozycji klastra. To jest ostateczna
		//	wartosc centrum dla klatki n, uzywana do transformacji do klatki n + 1
		//	w - wspolczynnik kontrolny w przedziale[0.02 - 0.2], wstepna wartosc 0.1

		//	C' = C + (P - C) * w

		auto P = getTransformed(*std::prev(i_Cluster->end()));
		auto C = i_PointToCorrent;

		i_PointToCorrent.x = C.x + (P.x - C.x) * CENTER_PARAM_W;
		i_PointToCorrent.y = C.y + (P.y - C.y) * CENTER_PARAM_W;
	}


	/* ---------------------------------------------------------- */
	cv::Point3f PathEstimator::getAverageTranslation(const std::vector<cv::Point3f>& i_Points_1, const std::vector<cv::Point3f>& i_Points_2) const
	{
		assert(i_Points_1.size() == i_Points_2.size());

		cv::Point3f average(0, 0, 0);

		size_t numPairs = i_Points_1.size();
		if (numPairs > 0)
		{
			for (int i = 0; i < numPairs; i++)
				average += (i_Points_2[i] - i_Points_1[i]);
			average /= static_cast<float>(numPairs);
		}

		return average;
	}

	Point PathEstimator::getTransformed(const Point& point) const
	{
		cv::Point2f tmp = getTransformed(cv::Point2f(point.x, point.y));
		return Point(tmp.x, tmp.y, point.t, point.n);
	}
}