#include "MonitorGraphicsRegion.h"
#include <assert.h>
#include "MonitorGraphicsRegionGroup.h"
#include "MonitorRegionsManager.h"

MonitorGraphicsRegion::MonitorGraphicsRegion(MonitorRegionPtr& source, QGraphicsItem *parent)
	: QGraphicsItem(parent)
	, m_Source(source)
	, m_State(NONE)
	, m_CloseState(OPEN)
	, m_BrushOpacity(0.2f)
	, m_OutlinePointRadius(5)
	, m_NormalPen(QPen(Qt::yellow, 5))
	, m_AlertPen(QPen(Qt::red, 10))
	, m_IsCollided(false)

{
	m_Parent = dynamic_cast<MonitorGraphicsRegionGroup*>(parent);
	m_AnimationPen = m_NormalPen;
	connect(&m_AlertAnimation, SIGNAL(valueChanged(QVariant)), this, SLOT(onAlertAnimationChanged(QVariant)));

	MonitorRegionsManager::getInstance().addListener(this);
}


MonitorGraphicsRegion::~MonitorGraphicsRegion()
{
	removePoints();
	MonitorRegionsManager::getInstance().removeListener(this);
}


void MonitorGraphicsRegion::setPoints(const RegionPoints& i_Points)
{
	if (!m_Points.isEmpty())
		removePoints();

	for (auto& point : i_Points) 
	{
		GraphicsPoint* gPoint = new GraphicsPoint(this, point);
		m_Points.push_back(gPoint);
	}

	recalcPolygon();
}


void MonitorGraphicsRegion::removePoints()
{
	for (GraphicsPoint* point : m_Points) 
		delete point;

	m_Points.clear();

	recalcPolygon();
}


MonitorGraphicsRegion::GraphicsPoint* MonitorGraphicsRegion::addPoint(const QPointF& _point)
{
	GraphicsPoint* gPoint = new GraphicsPoint(this, _point);
	m_Points.push_back(gPoint);

	recalcPolygon();

	return gPoint;
}


bool MonitorGraphicsRegion::removePoint(GraphicsPoint* i_PointToRemove)
{
	int index = m_Points.indexOf(i_PointToRemove);
	if (index != -1)
	{
		m_Points.remove(index);
		delete i_PointToRemove;
		recalcPolygon();
		return true;
	}
	
	return false;
}


bool MonitorGraphicsRegion::removePoint(size_t index)
{
	if (index >= 0 && index < m_Points.size())
		return removePoint(m_Points.at((int)index));
		
	return false;
}

void MonitorGraphicsRegion::setState(EState _state)
{
	if (m_State != _state) {
		m_State = _state;

		switch (m_State){ 

			case NONE:
				break;

			case CREATING:
				break;
		}
	}
}

void MonitorGraphicsRegion::setCloseState(ECloseState _state)
{
	if (m_CloseState != _state) {
		m_CloseState = _state;

		switch (m_CloseState){

		case OPEN:
			break;

		case HALF_CLOSE:
			break;

		case CLOSE:
			break;
		}

		m_Parent->onChanged(this);
	}
}


void MonitorGraphicsRegion::onRegionEnter(const MonitorRegionPtr& i_Region)
{
	if (i_Region->getType() == MonitorRegion::ALERT)
	{
		if (m_Source == i_Region)
		{
			m_AnimationPen = m_NormalPen;
			m_AlertAnimation.setStartValue(m_NormalPen.color());
			m_AlertAnimation.setEndValue(m_AlertPen.color());
			m_AlertAnimation.setDuration(1000);
			m_AlertAnimation.setEasingCurve(QEasingCurve::SineCurve);
			m_AlertAnimation.setLoopCount(-1);
			m_AlertAnimation.start();

			m_IsCollided = true;
		}
	}
	
}

void MonitorGraphicsRegion::onRegionLeave(const MonitorRegionPtr& i_Region)
{
	if (i_Region->getType() == MonitorRegion::ALERT)
	{
		if (m_Source == i_Region)
		{
			m_AlertAnimation.stop();
			m_IsCollided = false;
		}
	}
}


void MonitorGraphicsRegion::onMouseTracking(QGraphicsSceneMouseEvent *event)
{
	if (m_State == CREATING)
	{
		if (!m_Points.empty())
		{
			GraphicsPoint* lastPoint = m_Points.back();
			lastPoint->setPoint(event->scenePos());

			updateCloseState();
			recalcPolygon();
			scene()->update();
		}
	}
}

void MonitorGraphicsRegion::onMousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_State == CREATING)
	{
		if (m_CloseState == OPEN)
			addPoint(event->scenePos());
		else if (m_CloseState == HALF_CLOSE)
		{
			setCloseState(CLOSE);
			removePoint(m_Points.back());
		}
	}
}

void MonitorGraphicsRegion::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(m_IsCollided ? m_AnimationPen : m_NormalPen);

	//draw polygon lines
	if (m_Points.size() > 1)
	{
		GraphicsPoint* firstPoint = m_Points.at(0);
		for (int i = 1; i < m_Points.size(); i++)
		{
			GraphicsPoint* secondPoint = m_Points.at(i);
			painter->drawLine(firstPoint->getPoint(), secondPoint->getPoint());
			firstPoint = secondPoint;
		}

		if (m_CloseState == CLOSE)
		{
			painter->drawLine(m_Points.back()->getPoint(), m_Points.front()->getPoint());
		}


		//draw brush
		if (m_CloseState != OPEN)
		{
			painter->setBrush(m_IsCollided ? m_AnimationPen.color() : m_NormalPen.color());
			painter->setOpacity(m_IsCollided ? 0.4 : m_BrushOpacity);
			painter->drawPolygon(m_Source->getPolygon());
			painter->setOpacity(1);
		}
	}
}


void MonitorGraphicsRegion::recalcPolygon()
{
	QPolygonF poly;
	for (GraphicsPoint* point : m_Points)
		poly.push_back(point->getPoint());

	m_Source->setPolygon(poly);
}


void MonitorGraphicsRegion::updateCloseState()
{
	if (m_CloseState != CLOSE)
	{
		if (m_Points.size() > 2)
		{
			GraphicsPoint* firstPoint = m_Points.front();
			GraphicsPoint* secondPoint = m_Points.back();

			if (firstPoint->collidesWithItem(secondPoint))
				setCloseState(HALF_CLOSE);
			else
				setCloseState(OPEN);
		}
	}
}


void MonitorGraphicsRegion::onAlertAnimationChanged(const QVariant& variant)
{
	QColor color = m_AlertAnimation.currentValue().value<QColor>();
	m_AnimationPen.setColor(color);
	scene()->update(m_Source->getPolygon().boundingRect());
}


QRectF MonitorGraphicsRegion::boundingRect() const {
	if (scene())
		return scene()->sceneRect();
	return QRectF();
}




/* ---------------------------------- GRAPHICS POINT ------------------------------------- */
MonitorGraphicsRegion::GraphicsPoint::GraphicsPoint(QGraphicsItem* parent /*= 0*/, const QPointF& point /*= QPointF()*/)
	: QGraphicsEllipseItem(parent)
{

	m_Parent = dynamic_cast<MonitorGraphicsRegion*>(parent);
	assert(m_Parent);

	setPoint(point);

	//setFlag(QGraphicsItem::ItemIsMovable);
	//setFlag(QGraphicsItem::ItemIsFocusable);
	//setFlag(QGraphicsItem::ItemIsSelectable);
}


void MonitorGraphicsRegion::GraphicsPoint::setPoint(const QPointF& i_Point){
	m_Point = i_Point;
	int radius = m_Parent->m_OutlinePointRadius;
	setRect(-radius, -radius, 2 * radius, 2 * radius);
	setPos(i_Point.x(), i_Point.y());
}


QPointF MonitorGraphicsRegion::GraphicsPoint::getPoint() const {
	return m_Point;
}


void MonitorGraphicsRegion::GraphicsPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	setPen(m_Parent->m_IsCollided ? m_Parent->m_AnimationPen : m_Parent->m_NormalPen);
	setBrush(QBrush(m_Parent->m_IsCollided ? m_Parent->m_AnimationPen.color() : m_Parent->m_NormalPen.color()));

	painter->setRenderHint(QPainter::Antialiasing);
	QGraphicsEllipseItem::paint(painter, option, widget);
}


void MonitorGraphicsRegion::GraphicsPoint::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsEllipseItem::mouseMoveEvent(event);
	m_Point = event->scenePos();
	m_Parent->recalcPolygon();
}
