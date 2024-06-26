#ifndef __PATH_H__
#define __PATH_H__

#include <opencv2/opencv.hpp>
#include <utils/CvZoom.h>


/// <summary>
/// klasa przechowuj¹ca informacje o pojedynczej cie¿ce.
/// </summary>
class path 
{
public:
	enum DrawMode
	{
		MODE_LINE_POINT = 0,
		MODE_LINE,
		MODE_POINT,
	};

	// intended constructor
	path(cv::KeyPoint firstPoint, int maxLength, int maxMissedFrames, float* descriptor, int descriptorSize, cv::Point2f firstWorldPoint);
	~path(void);

	void draw(utils::ZoomObjectCollection& zoomCollection, float factor, DrawMode draw_mode);

	float* getMatchingDescriptor()const	{ return m_MatchingDescriptor; }
	cv::KeyPoint getMatchingPoint() const { return m_MatchingPoint; }

	cv::Point2f getPredictedNextPoint() const { return m_PredictedNextPoint; }
	cv::Point2f getPredictedNextWorldPoint() const { return m_PredictedNextWorldPoint; }

	unsigned long getID() const { return m_ID; }
	static void resetId() { ms_PathId = 0; }

	bool validateWithPredictedNextPoint(cv::Point2f newPt, float maxDistFromPredictedNextPoint);
	bool validateWithPredictedNextWorldPoint(cv::Point2f newWorldPt, float maxDistFromPredictedNextWorldPoint);
	int pushPoint(cv::KeyPoint newPoint, float* descriptor, cv::Point2f newWorldPoint);;

	int getPushedPoint() const { return _pushedPoint; }

	static float L2Dist(cv::Point2f pt1, cv::Point2f pt2);

	int getLength() const { return m_Length - m_MissedFrames; }
	float getMaxDistFromBegin() const { return m_MaxDistFromBegin; }

	//const cv::Point2f& getLastPoint() const { return m_Points[0]; }
	//const cv::Point2f& getLastWorldPoint() const { return m_WorldPoints[0]; }

	const cv::Point2f& getPoint(int index) const { return m_Points[index]; }
	const cv::Point2f& getWorldPoint(int index) const { return m_WorldPoints[index]; }

	int getMaxLength() const { return m_MaxLength; }

private:
	unsigned long m_ID;
	int m_MaxLength;
	int m_MaxMissedFrames;
	int m_MissedFrames;
	int m_DescriptorSize;

	cv::KeyPoint m_MatchingPoint;
	float* m_MatchingDescriptor;

	cv::Point2f* m_Points;
	cv::Point2f* m_WorldPoints;

	cv::Point2f m_PredictedNextPoint;
	cv::Point2f m_PredictedNextWorldPoint;

	cv::Point2f m_FirstPoint;
	int m_Length;
	float m_MaxDistFromBegin;

	static int ms_PathId;
	static const cv::Scalar c_Colors[];
	static const int c_NumColors;

public:
	int _pushedPoint;

};

#endif // __PATH_H__