#include "MonitorGraphicsView.h"
#include "MonitorVideoManager.h"

#include <QtWidgets/QScrollBar>


double MonitorGraphicsView::SCALE_FACTOR = 1.15;

MonitorGraphicsView::MonitorGraphicsView(QWidget *parent)
	: QGraphicsView(parent)
	, m_Scale(1)
{
	setTransformationAnchor(AnchorUnderMouse);
}


MonitorGraphicsView::~MonitorGraphicsView()
{

}

void MonitorGraphicsView::updateSceneRect(const QRectF& rect)
{
	if (m_Scale == 1)
		fitScene();

	QGraphicsView::updateSceneRect(rect);
}

void MonitorGraphicsView::resizeEvent(QResizeEvent *e)
{
	if (m_Scale == 1)
		fitScene();

	QGraphicsView::resizeEvent(e);
	
	emit sizeChanged(size());
}


void MonitorGraphicsView::fitScene(){
	fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
	m_Scale = 1;
}

void MonitorGraphicsView::wheelEvent(QWheelEvent *event)
{
	double scaleFactor = SCALE_FACTOR;
	if (event->delta() < 0) {
		if (m_Scale <= 1) return;
		scaleFactor = 1.0 / scaleFactor;
	}

	m_Scale *= scaleFactor;
	if (m_Scale < SCALE_FACTOR) {
		m_Scale = 1;
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

