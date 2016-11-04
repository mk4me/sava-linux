#ifndef MASKVIEW_H
#define MASKVIEW_H

#include <QtWidgets/QGraphicsView>
#include <QtGui/QWheelEvent>
#include <QtCore/QDebug>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsItem>



class MaskScene : public QGraphicsScene {

public:
	MaskScene(QObject* parent = 0) : QGraphicsScene(parent){}

	class Item : public QGraphicsItem {
		public:
			Item(QGraphicsItem* parent = 0) : QGraphicsItem(parent) {}

			virtual void mouseMove(QGraphicsSceneMouseEvent* e) {};
			virtual void mousePress(QGraphicsSceneMouseEvent *e) {}

		protected:
			virtual QRectF boundingRect() const override;
			virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override {}
	};

protected:
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

};



class MaskView : public QGraphicsView
{
	Q_OBJECT

	static double SCALE_FACTOR;
	static double MIN_SCALE;

public:
	MaskView(QWidget *parent = 0);

	void fitScene();

protected:
	virtual void wheelEvent(QWheelEvent *) override;
	virtual void resizeEvent(QResizeEvent *) override;

private:

	//scene scale
	double m_Scale;
	
};

#endif // MASKVIEW_H
