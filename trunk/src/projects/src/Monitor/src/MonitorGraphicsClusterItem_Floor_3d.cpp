#include "MonitorGraphicsClusterItem_Floor_3d.h"


MonitorGraphicsClusterItem_Floor_3d::MonitorGraphicsClusterItem_Floor_3d(QGraphicsItem* parent)
	:MonitorGraphicsClusterItem_Fill_2d(parent)
{
}

QVector<QPointF> MonitorGraphicsClusterItem_Floor_3d::createBoxVertexes()
{
	float sceneH = scene()->sceneRect().height();

	int w = m_Rect.width();

	QPointF offset;
	offset.setX(w / 6.0);
	offset.setY(w / 5.0 * (m_Rect.bottom() / sceneH + 0.3)  );
	QPointF offsetUp(0, m_Rect.height());
	QPointF padding(12, 0);

	QPointF downFrontLeft, downFrontRight, downBackRight, downBackLeft;
	QPointF upFrontLeft, upFrontRight, upBackRight, upBackLeft;

	downFrontLeft = m_Rect.bottomLeft() - padding + offset;// *0.5;
	downFrontRight = m_Rect.bottomRight() + padding + offset;// *0.5;
	downBackRight = m_Rect.bottomRight() + padding - offset;// *1.5;
	downBackLeft = m_Rect.bottomLeft() - padding - offset;// *1.5;

	upFrontLeft = downFrontLeft - offsetUp;
	upFrontRight = downFrontRight - offsetUp;
	upBackRight = downBackRight - offsetUp;
	upBackLeft = downBackLeft - offsetUp;

	QVector<QPointF> points;
	points << downFrontLeft;
	points << downFrontRight;
	points << downBackRight;
	points << downBackLeft;
	points << upFrontLeft;
	points << upFrontRight;
	points << upBackRight;
	points << upBackLeft;
	return points;
}

void MonitorGraphicsClusterItem_Floor_3d::drawWall(QVector<QPointF>& points, BoxType boxType, QPainter* painter)
{

	painter->save();

	QPainterPath path;
	QPolygonF poly;

	for (auto point : points)
		poly << point;

	if (boxType == FLOOR)
		poly << points[0];

	path.addPolygon(poly);

	QColor color(Qt::black);

	if (boxType == FLOOR) {
		painter->setOpacity(0.3);
		painter->fillPath(path, color);
	}

	painter->setOpacity(0.4);
	painter->setPen(QPen(color, 2));
	painter->drawPath(path);

	painter->restore();
}


void MonitorGraphicsClusterItem_Floor_3d::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{

	painter->setRenderHint(QPainter::Antialiasing);

	QVector<QPointF> poinst = createBoxVertexes();

	//draw floor
	drawWall(QVector<QPointF>() << poinst[0] << poinst[1] << poinst[2] << poinst[3], FLOOR, painter);

	//draw walls
	//drawWall(QVector<QPointF>() << poinst[4] << poinst[0] << poinst[3] << poinst[7], WALL, painter);
	//drawWall(QVector<QPointF>() << poinst[5] << poinst[1] << poinst[0] << poinst[4], WALL, painter);
	//drawWall(QVector<QPointF>() << poinst[5] << poinst[1] << poinst[2] << poinst[6], WALL, painter);
	//drawWall(QVector<QPointF>() << poinst[6] << poinst[2] << poinst[3] << poinst[7], WALL, painter);

	MonitorGraphicsClusterItem_Fill_2d::paint(painter, option, widget);
}



