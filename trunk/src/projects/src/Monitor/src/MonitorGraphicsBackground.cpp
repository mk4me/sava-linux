#include "MonitorGraphicsBackground.h"
#include "MonitorVideoManager.h"



MonitorGraphicsBackground::MonitorGraphicsBackground(QGraphicsItem *parent)
	: QGraphicsItem(parent)
{

}

MonitorGraphicsBackground::~MonitorGraphicsBackground()
{

}

void MonitorGraphicsBackground::update(size_t _frame)
{
	setImage(utils::image_converter::cvMatToQImage(MonitorVideoManager::getInstance().getMetaVideo()->getVideo()->getFrameImage(_frame)));
}

QRectF MonitorGraphicsBackground::boundingRect() const
{
	if (scene())
		return scene()->sceneRect();

	return QRectF();
}

void MonitorGraphicsBackground::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	painter->drawImage(option->rect, m_Image);
}
