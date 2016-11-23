#pragma once

#include "QtWidgets/QGraphicsItem"
#include "QtGui/QPainter"
#include "EditorSceneObject.h"

class EditorSceneObjectMeasure : public QGraphicsItem
{

public:
	EditorSceneObjectMeasure(QGraphicsItem* parent = nullptr);

	void setObject(EditorSceneObject* obj);
	void update();

protected:
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

private:
	EditorSceneObject* m_Object;

	QFont m_Font;
	QPen m_Pen;
	QBrush m_Brush;
};

