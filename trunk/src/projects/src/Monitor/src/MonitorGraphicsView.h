#ifndef VIDEOGRAPHICSVIEW_H
#define VIDEOGRAPHICSVIEW_H

#include <QtWidgets/QGraphicsView>
#include <QtCore/QDebug>
#include <QtGui/QWheelEvent>

class MonitorGraphicsView : public QGraphicsView
{
	Q_OBJECT;

	static double SCALE_FACTOR;

public:
	MonitorGraphicsView(QWidget *parent = 0);
	~MonitorGraphicsView();

	void fitScene();

signals:
	void sizeChanged(const QSize& newSize);

public slots:
	void updateSceneRect(const QRectF& rect);

protected:
	virtual void resizeEvent(QResizeEvent *) override;
	virtual void wheelEvent(QWheelEvent *) override;

private:

	//scene scale
	double m_Scale;

};

#endif // VIDEOGRAPHICSVIEW_H
