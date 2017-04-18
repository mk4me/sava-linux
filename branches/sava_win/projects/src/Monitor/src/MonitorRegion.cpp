#include "MonitorRegion.h"


MonitorRegion::MonitorRegion(MonitorRegion::EType type)
	: m_Type(type)
	, m_IsCollided(false)
{
	
}


MonitorRegion::MonitorRegion(const MonitorRegion& region)
{
	m_Polygon = region.m_Polygon;
	m_Type = region.m_Type;
}

MonitorRegion::~MonitorRegion()
{ 
	
}

bool MonitorRegion::isCollidedWith(const QRectF& rect) const
{
	int topLeftCollision = m_Polygon.containsPoint(rect.topLeft(), Qt::OddEvenFill);
	int topRightCollision = m_Polygon.containsPoint(rect.topRight(), Qt::OddEvenFill);
	int bottomLeftCollision = m_Polygon.containsPoint(rect.bottomLeft(), Qt::OddEvenFill);
	int bottomRightCollision = m_Polygon.containsPoint(rect.bottomRight(), Qt::OddEvenFill);

	int collisionPointCount = topLeftCollision + topRightCollision + bottomLeftCollision + bottomRightCollision;

	return (collisionPointCount > 1);
}