#ifndef MONITORGRAPHICSALERTREGIONITEM_H
#define MONITORGRAPHICSALERTREGIONITEM_H

#include <QtWidgets/QGraphicsItem>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtCore/QDebug>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtCore/QVariantAnimation>

#include "MonitorRegion.h"
#include "MonitorRegion.h"
#include "sequence/Cluster.h"


typedef QVector<QPointF> RegionPoints;


class MonitorGraphicsRegionGroup;


class MonitorGraphicsRegion : public QObject, public QGraphicsItem, public IRegionListener
{

	Q_OBJECT;
	//Q_INTERFACES(QGraphicsItem);

	class GraphicsPoint;
	friend class GraphicsPoint;

public:

	enum EState
	{
		NONE,
		CREATING,
	};

	enum ECloseState
	{
		OPEN, 
		HALF_CLOSE,
		CLOSE,
	};

	MonitorGraphicsRegion(MonitorRegionPtr& source, QGraphicsItem *parent = 0);
	virtual ~MonitorGraphicsRegion();

	virtual MonitorRegion::EType getType() = 0;

	MonitorRegionPtr getSource() { return m_Source; }

	size_t getPointCount() const { return m_Points.size(); }

	void setPoints(const RegionPoints& i_Points);
	void removePoints();

	GraphicsPoint* addPoint(const QPointF& _point);
	bool removePoint(GraphicsPoint* i_PointToRemove);
	bool removePoint(size_t index);

	void setState(EState _state);
	EState getState() const { return m_State; }

	void setCloseState(ECloseState _state);
	ECloseState getCloseState() const { return m_CloseState; }

	void onMouseTracking(QGraphicsSceneMouseEvent *event);
	void onMousePressEvent(QGraphicsSceneMouseEvent *event);

	//visualization functionality
	void setRegionOpacity(int _opacity) { m_BrushOpacity = _opacity; }
	void setRegionPointRadius(int _radius) { m_OutlinePointRadius = _radius; }
	void setRegionNormalPen(const QPen& _pen) { m_NormalPen = _pen; }
	void setRegionAlertPen(const QPen& _pen) { m_AlertPen = _pen; }

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
	virtual QRectF boundingRect() const override;

	virtual void onRegionEnter(const MonitorRegionPtr& i_Region) override;
	virtual void onRegionLeave(const MonitorRegionPtr& i_Region) override;

private slots:
	void onAlertAnimationChanged(const QVariant& variant);

private:
	void recalcPolygon();
	void updateCloseState();

private:
	MonitorRegionPtr m_Source;
	MonitorGraphicsRegionGroup* m_Parent;

	QVector<GraphicsPoint*> m_Points;

	EState m_State;
	ECloseState m_CloseState;
	bool m_IsCollided;

	//visualization params
	int m_OutlinePointRadius;
	float m_BrushOpacity;
	QPen m_NormalPen;
	QPen m_AlertPen;

	//alert animation
	QPen m_AnimationPen;
	QVariantAnimation m_AlertAnimation;


	//object represents region point
	class GraphicsPoint : public QGraphicsEllipseItem
	{
		public:
			GraphicsPoint(QGraphicsItem* parent = 0, const QPointF& point = QPointF());

			void setPoint(const QPointF& i_Point);
			QPointF getPoint() const;

		protected:
			virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
			virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

		private:
			QPointF m_Point;
			MonitorGraphicsRegion* m_Parent;
	};
};




//ALERT GRAPHICS REGION
class MonitorGraphicsRegionAlert: public MonitorGraphicsRegion
{

public :
	MonitorGraphicsRegionAlert(MonitorRegionPtr& source, QGraphicsItem *parent = 0) : MonitorGraphicsRegion(source, parent){ }

	virtual MonitorRegion::EType getType() { return MonitorRegion::ALERT; }

};



//MASK GRAPHICS REGION
class MonitorGraphicsRegionMask : public MonitorGraphicsRegion
{

public:
	MonitorGraphicsRegionMask(MonitorRegionPtr& source, QGraphicsItem *parent = 0) : MonitorGraphicsRegion(source, parent) {
		setRegionNormalPen(QPen(Qt::black, 5));
		setRegionOpacity(1);
	}

	virtual MonitorRegion::EType getType() { return MonitorRegion::MASK; }

protected:
	virtual void onRegionEnter(const MonitorRegionPtr& i_Region) override { }
	virtual void onRegionLeave(const MonitorRegionPtr& i_Region) override { }

};

#endif // MONITORGRAPHICSALERTREGIONITEM_H
