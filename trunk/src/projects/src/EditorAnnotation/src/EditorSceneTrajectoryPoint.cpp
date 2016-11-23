#include "EditorSceneTrajectoryPoint.h"
#include "QtGui/QBrush"
#include <QtCore/QDebug>
//#include "../src/gui/kernel/qevent.h"
#include <QtCore/QEvent>
#include "EditorSceneTrajectory.h"
#include "EditorTimeline.h"

/*--------------------------------------------------------------------*/
EditorSceneTrajectoryPoint::EditorSceneTrajectoryPoint(const EditorTrajectoryPointPtr& i_SourcePoint, QGraphicsItem* parent /*= 0*/)
	: QGraphicsItem(parent),
	 m_SourcePoint(i_SourcePoint)
{
	m_Parent = dynamic_cast<EditorSceneTrajectory*>(parent);
	assert(m_Parent);

	setFlag(QGraphicsItem::ItemIsSelectable);
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges);

	setPos(i_SourcePoint->getX(), i_SourcePoint->getY());
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectoryPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setPen(QPen(Qt::black));
	painter->setOpacity(1);

	if (isSelected())
		painter->setBrush(QBrush(Qt::red));
	else
		painter->setBrush(m_Parent->getColor());

	painter->drawEllipse(boundingRect());
}
//
/*--------------------------------------------------------------------*/
QRectF EditorSceneTrajectoryPoint::boundingRect() const
{
	int radius = EditorConfig::getInstance().getTrajectoryPointRadius(); 
	return QRectF(-radius, -radius, 2 * radius, 2 * radius);
}

/*--------------------------------------------------------------------*/
QVariant EditorSceneTrajectoryPoint::itemChange(GraphicsItemChange change, const QVariant &value)
{
	QPointF newPos = value.toPoint();

	if (change == ItemPositionChange)
	{
		if (scene())
		{
			QRectF rect = scene()->sceneRect();
			if (!rect.contains(newPos)) 
			{
				// Keep the item inside the scene rect.
				newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
				newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
			}
		}

		m_SourcePoint->setPos(newPos.x(), newPos.y());
		return newPos;
	}

	return QGraphicsItem::itemChange(change, value);
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectoryPoint::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	parentItem()->update();
	QGraphicsItem::mouseMoveEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorSceneTrajectoryPoint::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	//go to point time
	//REMOVE 
	/*int currentTime = m_SourcePoint->getTime() - EditorCrumbledStreamData::getInstance().getSequencesRemovedFrames();
	if (currentTime >= 0)
		EditorTimeline::getInstance().setCurrentFrame(currentTime);
	else
		qDebug() << "Point is from another sequence.";*/

	QGraphicsItem::mousePressEvent(event);
}