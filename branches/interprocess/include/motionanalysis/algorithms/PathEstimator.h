#pragma once
#ifndef PathEstimator_h__
#define PathEstimator_h__

#include <list>
#include <memory>

#include <motionanalysis/algorithms/PathClustering.h>
//#include "camera/calibration/algorithms/Transforms.h"

namespace motion_analysis
{

	class PathEstimator
	{

	public:

		typedef std::map<PathTimeType, cv::Rect> BoxesMap;

		static float CENTER_PARAM_THRESHOLD; // pixels (squared)
		static float CENTER_PARAM_W; //przedzial [0.02 - 0.2]

		static int BOUNDING_BOX_PARAM_R;  //przedzial [2 - 15]
		static int BOUNDING_BOX_PARAM_T;  //przedzial [4 - 20]
		static int BOUNDING_BOX_DEFAULT_FRAMES_COUNT;


		enum EstimatedMethod
		{
			FROM_NO_PAIR,
			FROM_ONE_PAIR,
			FROM_TWO_PAIRS,
			FROM_THREE_PAIRS,
			FROM_THREE_PAIRS_ALTERNATIVE,
		};

		PathEstimator(const cv::Mat1d& t = cv::Mat1d::eye(3, 3));

		void update(const std::list<std::shared_ptr<PathCluster>> &clusters, PathTimeType time);
		void drawPoint(const std::shared_ptr<PathCluster>& i_Cluster, cv::Mat& i_Image, const cv::Scalar& i_Color) const;
		void drawBoundingBox(const std::shared_ptr<PathCluster>& i_Cluster, cv::Mat& i_Image, PathTimeType i_Time);

		cv::Rect getBoundingBox(
			const std::shared_ptr<PathCluster>& cluster,
			PathTimeType endTime,
			size_t frameCount = 0) const;

		BoxesMap getBoundingBoxes(
			const std::shared_ptr<PathCluster>& cluster,
			PathTimeType endTime,
			size_t frameCount = 0);

	protected:
		bool calcFromOnePair(
			const std::vector<cv::Point3f>& i_Points_1,
			const std::vector<cv::Point3f>& i_Points_2,
			const Point& i_PrevPoint, Point& o_NextPoint) const;

		bool calcFromTwoPairs(
			const std::vector<cv::Point3f>& i_Points_1,
			const std::vector<cv::Point3f>& i_Points_2,
			const Point& i_PrevPoint, Point& o_NextPoint) const;

		bool calcFromThreePairs_1(
			const std::vector<cv::Point3f>& i_Points_1,
			const std::vector<cv::Point3f>& i_Points_2,
			const Point& i_PrevPoint, Point& o_NextPoint) const;

		bool calcFromThreePairs_2(
			const std::vector<cv::Point3f>& i_Points_1,
			const std::vector<cv::Point3f>& i_Points_2,
			const Point& i_PrevPoint, Point& o_NextPoint) const;

		void escapeElimination(
			const std::shared_ptr<PathCluster>& i_Cluster,
			Point& i_PointToCorrent) const;

	private:
		cv::Point3f getAverageTranslation(const std::vector<cv::Point3f>& i_Points_1, const std::vector<cv::Point3f>& i_Points_2) const;

		cv::Mat1d m_Transform;

		template<typename T>
		cv::Point_<T> getTransformed(const cv::Point_<T>& point) const;

		template<typename T>
		cv::Point3_<T> getTransformed(const cv::Point3_<T>& point) const;

		motion_analysis::Point getTransformed(const motion_analysis::Point& point) const;
		//void transform(cv::Point& point) const;
		//void transform(cv::Point3f& point) const;
		//void transform(const motion_analysis::Point& point) const;

	};

	template<typename T>
	cv::Point_<T> PathEstimator::getTransformed(const cv::Point_<T>& point) const
	{
		// TODO dodac normalizacje
		//return calibration::Transforms::plane2Plane(point, m_Transform);
		return point;
	}

	template<typename T>
	cv::Point3_<T> PathEstimator::getTransformed(const cv::Point3_<T>& point) const
	{
		cv::Point_<T> tmp = getTransformed(cv::Point_<T>(point.x, point.y));
		return cv::Point3_<T>(tmp.x, tmp.y, 0);
	}
}

#endif // PathEstimator_h__