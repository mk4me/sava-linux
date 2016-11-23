#include "EditorSceneObjectMeasure.h"
#include <QtCore/QDebug>


EditorSceneObjectMeasure::EditorSceneObjectMeasure(QGraphicsItem* parent /*= nullptr*/)
	: QGraphicsItem(parent)
	, m_Object(nullptr)
{
	m_Pen.setColor(Qt::green);
	m_Font.setBold(true);
	m_Brush.setColor(Qt::black);
	m_Brush.setStyle(Qt::SolidPattern);

	setVisible(false);
}


void EditorSceneObjectMeasure::setObject(EditorSceneObject* obj)
{
	m_Object = obj;
	setVisible(m_Object);
}

void EditorSceneObjectMeasure::update()
{
	if (m_Object)
		setPos(m_Object->rect().bottomRight() + QPoint(5, 5) );

	QGraphicsItem::update();
}

QRectF EditorSceneObjectMeasure::boundingRect() const{
	return QRectF(0, 0, 150, 20);
}

void EditorSceneObjectMeasure::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	if (m_Object)
	{
		painter->setRenderHint(QPainter::TextAntialiasing);

		QRectF bb = boundingRect();

		painter->setBrush(m_Brush);
		painter->drawRoundedRect(bb, 5, 5);
	
		QRect r = m_Object->rect().toRect(); 
		QString text = QString("%1 , %2  %3 x %4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height());

		painter->setPen(m_Pen);
		painter->setFont(m_Font);
		painter->drawText(bb, Qt::AlignCenter, text);
	}	
}


