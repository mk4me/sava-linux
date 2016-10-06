#pragma once
#include "QtWidgets\QGraphicsItem"
#include "QtGui\QPainter"
#include "QtWidgets/QGraphicsScene"
#include "EditorTrajectoryPoint.h"
#include "QtWidgets/QGraphicsSceneEvent"
#include "EditorSingletons.h"

class EditorSceneTrajectory;

/**
 * \class EditorSceneTrajectoryPoint
 *
 * \brief Represents graphic trajectory point in editor scene.
 *
 */
class EditorSceneTrajectoryPoint : public QGraphicsItem
{

public:
	EditorSceneTrajectoryPoint(const EditorTrajectoryPointPtr& i_SourcePoint, QGraphicsItem* parent = 0);

	EditorTrajectoryPointPtr getSourcePoint() { return m_SourcePoint; }

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
	virtual QRectF boundingRect() const override;
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
	EditorTrajectoryPointPtr m_SourcePoint;
	EditorSceneTrajectory* m_Parent;
};