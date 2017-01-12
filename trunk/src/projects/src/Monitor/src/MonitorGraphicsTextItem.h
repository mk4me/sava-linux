#pragma once
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsView>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "sequence/IVideo.h"


/// <summary>
/// Klasa wywietlaj¹ca dowolny tekst na scenie Monitora.
/// </summary>
class MonitorGraphicsTextItem : public QObject, public QGraphicsItem
{
	Q_OBJECT;
	//Q_INTERFACES(QGraphicsItem)

public:
	MonitorGraphicsTextItem(QGraphicsItem* parent = 0);

	void init();
	virtual void update(size_t _frame);

	void setRect(const QRect& rect) { m_Rect = rect; }
	void setText(const QString& text) { m_Text = text; }
	void setAlignmentFlag(Qt::AlignmentFlag alignment) { m_AlignmentFlag = alignment; }

	QRectF getMappedRect() const { return m_MappedRect; }

private slots:
	void onViewSizeChanged(const QSize& newSize);

protected:
	void repaintRect();
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
	virtual QRectF boundingRect() const override;

private:
	QGraphicsView *m_GraphView;

	QString m_Text;
	QRect m_Rect;
	QRectF m_MappedRect;
	Qt::AlignmentFlag m_AlignmentFlag;
};

