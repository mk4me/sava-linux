#include "MaskView.h"
#include <QtWidgets/QScrollBar>
#include <QtCore/QDebug>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>

/********************************** MASK SCENE **************************************/
QRectF MaskScene::Item::boundingRect() const
{
	if (scene())
		return scene()->sceneRect();

	return QRectF();
}


void MaskScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mouseMoveEvent(event);

	for (QGraphicsItem* i : items()) {
		MaskScene::Item* item = dynamic_cast<MaskScene::Item*>(i);
		if (item)
			item->mouseMove(event);
	}


}


void MaskScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mousePressEvent(event);

	for (QGraphicsItem* i : items()) {
		MaskScene::Item* item = dynamic_cast<MaskScene::Item*>(i);
		if (item)
			item->mousePress(event);
	}
}

/*********************************** MASK VIEW *************************************/
double MaskView::SCALE_FACTOR = 1.1;
double MaskView::MIN_SCALE = 0.9;

MaskView::MaskView(QWidget *parent)
	: QGraphicsView(parent)
	, m_Scale(MIN_SCALE)
{
	setTransformationAnchor(AnchorUnderMouse);
}


void MaskView::resizeEvent(QResizeEvent *e) {
	if (m_Scale == MIN_SCALE)
		fitScene();

	QGraphicsView::resizeEvent(e);
}


void MaskView::fitScene(){
	fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
	m_Scale = MIN_SCALE;
	scale(m_Scale, m_Scale);
}

void MaskView::wheelEvent(QWheelEvent *event)
{
	double scaleFactor = SCALE_FACTOR;
	if (event->delta() < 0) {
		if (m_Scale <= MIN_SCALE) return;
		scaleFactor = 1.0 / scaleFactor;
	}

	m_Scale *= scaleFactor;
	if (m_Scale < MIN_SCALE) {
		fitScene();
	}
	else {
		scale(scaleFactor, scaleFactor);
	}

	setDragMode(m_Scale > 1 ? ScrollHandDrag : NoDrag);

	event->ignore();
	QWheelEvent overridEvent(QPoint(0, 0), 0, Qt::NoButton, Qt::NoModifier);
	QGraphicsView::wheelEvent(&overridEvent);
}
