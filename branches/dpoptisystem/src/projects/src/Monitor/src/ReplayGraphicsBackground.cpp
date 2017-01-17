#include "ReplayGraphicsBackground.h"
#include "utils/ImageConverter.h"


ReplayGraphicsBackground::ReplayGraphicsBackground(QGraphicsItem *parent)
	: MonitorGraphicsBackground(parent)
{
}


ReplayGraphicsBackground::~ReplayGraphicsBackground()
{
}

void ReplayGraphicsBackground::update(size_t _frame)
{
	setImage(utils::image_converter::cvMatToQImage(m_Video->getFrameImage(_frame)));
}
