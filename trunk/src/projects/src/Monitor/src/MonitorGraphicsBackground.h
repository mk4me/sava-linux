#ifndef MONITORGRAPHICSBACKGROUND_H
#define MONITORGRAPHICSBACKGROUND_H

#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QStyleOption>
#include <QtGui/QPainter>
#include <QtGui/QImage>

#include "utils/ImageConverter.h"
#include "sequence/Video.h"


class MonitorGraphicsBackground : public QGraphicsItem
{
public:
	MonitorGraphicsBackground(QGraphicsItem *parent = 0);
	~MonitorGraphicsBackground();

	virtual void update(size_t _frame);

	void setImage(const QImage& _image) { m_Image = _image; }

protected:
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

private:
	QImage m_Image;
};

#endif // MONITORGRAPHICSBACKGROUND_H
