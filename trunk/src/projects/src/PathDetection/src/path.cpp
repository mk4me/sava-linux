#include "path.h"

using namespace cv;


int path::ms_PathId = 0;

const Scalar path::c_Colors[] =
{
	CV_RGB(253, 51, 0),
	CV_RGB(255, 0, 0),
	CV_RGB(255, 51, 204),
	CV_RGB(204, 153, 255),
	CV_RGB(153, 51, 255),
	CV_RGB(153, 204, 255),
	CV_RGB(0, 0, 255),
	CV_RGB(102, 255, 204),
	CV_RGB(204, 255, 153),
	CV_RGB(0, 128, 0),
};
const int path::c_NumColors = sizeof(c_Colors) / sizeof(*c_Colors);

path::path(KeyPoint firstPoint, int maxLength, int maxMissedFrames, float* descriptor, int descriptorSize, Point2f firstWorldPoint)
{
	// mark that a point was added to this path in current frame
	_pushedPoint = 1;
	m_ID = ms_PathId++;
	m_MatchingPoint = firstPoint;

	m_MaxLength = maxLength;
	m_MaxMissedFrames = maxMissedFrames;

	m_Points = (Point2f*)malloc(maxLength*sizeof(Point2f));
	m_Points[0] = firstPoint.pt;

	m_WorldPoints = (Point2f*)malloc(maxLength*sizeof(Point2f));
	m_WorldPoints[0] = firstWorldPoint;

	for (int i = 1; i < m_MaxLength; ++i) {
		m_Points[i] = Point2f(-1.0, -1.0);
		m_WorldPoints[i] = Point2f(-1.0, -1.0);
	}

	m_MissedFrames = 0;

	m_DescriptorSize = descriptorSize;
	m_MatchingDescriptor = (float*)malloc(m_DescriptorSize*sizeof(float));

	// TO DO: after correctness confirmation switch to memcpy
	for (int i = 0; i < m_DescriptorSize; ++i) {
		m_MatchingDescriptor[i] = descriptor[i];
	}

	m_PredictedNextPoint = firstPoint.pt;
	m_PredictedNextWorldPoint = firstWorldPoint;

	m_FirstPoint = firstWorldPoint;
	m_Length = 1;
	m_MaxDistFromBegin = 0.0f;
}

path::~path(void)
{
	free(m_Points);
	free(m_WorldPoints);
	free(m_MatchingDescriptor);
}

void path::draw(utils::ZoomObjectCollection& zoomCollection, float factor, DrawMode draw_mode)
{
	// First check the actual length of path as the number of valid points in it
	int pathLen = 0;

	for (int i = 0; i < m_MaxLength; ++i) {
		Point currPt = m_Points[i];

		if (currPt.x >= 0 && currPt.y >= 0) {
			++pathLen;
		}
	}

	Scalar color = c_Colors[m_ID % c_NumColors];

	Point prevPt = m_Points[m_MissedFrames];
	prevPt.x *= factor;
	prevPt.y *= factor;

	if (draw_mode == MODE_LINE_POINT || draw_mode == MODE_POINT) {
		//circle(img,prevPt,2,color);
		utils::zoomCircle(zoomCollection, prevPt, 2, color);
	}

	for (int i = m_MissedFrames + 1; i < m_MaxLength; ++i) {
		Point currPt = m_Points[i];

		if (currPt.x >= 0 && currPt.y >= 0) {
			currPt.x *= factor;
			currPt.y *= factor;

			if (draw_mode == MODE_LINE_POINT || draw_mode == MODE_POINT) {
				//circle(img,currPt,2,color);
				utils::zoomCircle(zoomCollection, currPt, 2, color);
			}

			if (draw_mode == MODE_LINE_POINT || draw_mode == MODE_LINE) {
				//line(img,prevPt,currPt,color);
				utils::zoomLine(zoomCollection, prevPt, currPt, color);
				prevPt = currPt;
			}
		}
	}
}

bool path::validateWithPredictedNextPoint(Point2f newPt, float maxDistFromPredictedNextPoint)
{
	float dist = L2Dist(newPt, m_PredictedNextPoint);
	return dist < maxDistFromPredictedNextPoint;
}

bool path::validateWithPredictedNextWorldPoint(Point2f newWorldPt, float maxDistFromPredictedNextWorldPoint)
{
	float dist = L2Dist(newWorldPt, m_PredictedNextWorldPoint);
	return dist < maxDistFromPredictedNextWorldPoint;
}

int path::pushPoint(KeyPoint newPoint, float* descriptor, Point2f newWorldPoint)
{
	// mark that a point was added to this path in current frame
	_pushedPoint = 1;


	// Push the FIFO queue (path) by one element to make room for the new point
	// Note that the last point in queue is being removed!!
	for (int i = m_MaxLength - 2; i >= 0; --i) {
		m_Points[i + 1] = m_Points[i];
		m_WorldPoints[i + 1] = m_WorldPoints[i];
	}

	// Push the new point (valid or not) to the FIFO queue (path)
	m_Points[0] = newPoint.pt;
	m_WorldPoints[0] = newWorldPoint;

	++m_Length;

	// Check if the new point is valid
	if (newPoint.pt.x < 0 || newPoint.pt.y < 0) {
		// If not increment the missed frames number;
		++m_MissedFrames;

		// We do not replace the matching point so that we will always match
		// to the _LAST_VALID_POINT_ in path

		// Check if the number of missed frames in current path is too large
		if (m_MissedFrames > m_MaxMissedFrames) {
			// If yes return signal to remove path
			return -1;
		}

	}
	else {
		if (m_MissedFrames == 0 && m_Points[1].x >= 0 && m_Points[1].y >= 0) {
			// Predict next point
			const Point2f prevPoint = m_Points[1];
			const Point2f currPoint = m_Points[0];

			const float dX = currPoint.x - prevPoint.x;
			const float dY = currPoint.y - prevPoint.y;

			m_PredictedNextPoint = Point2f(currPoint.x + dX, currPoint.y + dY);

			const Point2f prevWorldPoint = m_WorldPoints[1];
			const Point2f currWorldPoint = m_WorldPoints[0];

			const float worlddX = currWorldPoint.x - prevWorldPoint.x;
			const float worlddY = currWorldPoint.y - prevWorldPoint.y;

			m_PredictedNextWorldPoint = Point2f(currWorldPoint.x + worlddX, currWorldPoint.y + worlddY);


		}
		else {
			m_PredictedNextPoint = newPoint.pt;
			m_PredictedNextWorldPoint = newWorldPoint;
		}

		// If it is valid reset the number of missed frames for this path
		m_MissedFrames = 0;

		// Note that we do not want to keep the total number of missed frames!!
		// We just need to know if the path wasn't lost for too long

		// Replace the matching point
		m_MatchingPoint = newPoint;

		for (int i = 0; i < m_DescriptorSize; ++i) {
			m_MatchingDescriptor[i] = descriptor[i];
		}

		float dist = L2Dist(newWorldPoint, m_FirstPoint);
		if (m_MaxDistFromBegin < dist)
			m_MaxDistFromBegin = dist;
	}

	// signal that the path is valid
	return 0;
}

float path::L2Dist(Point2f pt1, Point2f pt2)
{
	const float diffX = pt1.x - pt2.x;
	const float diffY = pt1.y - pt2.y;
	return sqrt(diffX*diffX + diffY*diffY);
}

