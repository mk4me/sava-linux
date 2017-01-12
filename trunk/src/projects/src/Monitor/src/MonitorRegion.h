#pragma once

#include <QtGui/QPolygonF>
#include <QtCore/QDebug>

#include <memory>
#include "sequence/Cluster.h"

class MonitorRegion;
typedef std::shared_ptr<MonitorRegion> MonitorRegionPtr;

class IRegionListener
{
public:
	virtual void onRegionEnter(const MonitorRegionPtr& i_Region) = 0;
	virtual void onRegionLeave(const MonitorRegionPtr& i_Region) = 0;
};


/// <summary>
/// Klasa reprezentuj¹ca obiekt pojedynczego regionu.
/// </summary>
class MonitorRegion
{

public:

	enum EType
	{
		ALERT,
		MASK,
	};

	MonitorRegion(MonitorRegion::EType type = ALERT);
	~MonitorRegion();

	MonitorRegion(const MonitorRegion& region);

	EType getType() const { return m_Type; }
	void setType(EType type) { m_Type = type; }

	void setPolygon(const QPolygonF& polygon) { m_Polygon = polygon; }
	QPolygonF getPolygon() const { return m_Polygon; }

	bool isCollided() const { return m_IsCollided; }
	void setCollided(bool collided) { m_IsCollided = collided; }

	bool isCollidedWith(const QRectF& rect) const;

private:
	friend class boost::serialization::access;

	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
		ar >> m_Type;

		std::vector<std::pair<float, float>> points;
		ar >> points;

		for (auto& point : points)
			m_Polygon.push_back(QPointF(point.first, point.second));
	}

	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
		ar << m_Type;

		std::vector<std::pair<float, float>> points;
		for (QPointF point : m_Polygon)
			points.push_back(std::make_pair((float)point.x(), (float)point.y()));

		ar << points;
	}


	BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
	QPolygonF m_Polygon;
	EType m_Type;

	bool m_IsCollisionEnabled;
	bool m_IsCollided;
};

