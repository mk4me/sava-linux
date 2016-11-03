#include "MonitorGraphicsClusters.h"
#include "MonitorRegionsManager.h"



QColor MonitorGraphicsClusters::CLUSTER_COLOR = Qt::yellow;
QColor MonitorGraphicsClusters::ALERT_COLOR = Qt::red;

MonitorGraphicsClusters::MonitorGraphicsClusters(QGraphicsItem *parent)
	: QGraphicsItem(parent)
{
	m_ActionPen.setColor(CLUSTER_COLOR);
	m_ActionPen.setWidth(2);
	m_AlertPen.setColor(ALERT_COLOR);
	m_AlertPen.setWidth(2);
	m_TextFont.setPixelSize(14);
	m_TextFont.setBold(true);

	m_CachedVideoManger = MonitorVideoManager::getPointer();
	m_CachedActionManager = MonitorActionManager::getPointer();
}

MonitorGraphicsClusters::~MonitorGraphicsClusters()
{

}

void MonitorGraphicsClusters::update(size_t _frame)
{
	//set actions rectangle to draw
	m_ClustersInfo.clear();
	MonitorVideoManager::ActionPairVec vec = m_CachedVideoManger->getClusterActionPairs(_frame);
	for (auto it = vec.begin(); it != vec.end(); ++it)
	{
		cv::Rect rect = it->first->getFrameAt(_frame);

		ClusterInfo actionInfo;
		actionInfo.rect = QRectF(rect.x, rect.y, rect.width, rect.height); 

		if (MonitorRegionsManager::getInstance().isCollidedWith(MonitorRegion::MASK, actionInfo.rect))
			continue;

		if (it->second != nullptr)
		{
			int actionId = it->second->getActionId();
			actionInfo.name = QString(config::Glossary::getInstance().getTrainedActionName(actionId).c_str()).toUpper();
			actionInfo.isAlert = m_CachedActionManager->isAlert(actionId);
		}
		else
		{
			actionInfo.name = "";
			actionInfo.isAlert = false;
		}

		m_ClustersInfo.push_back(actionInfo);
	}
}


void MonitorGraphicsClusters::clear() {
	m_ClustersInfo.clear();
}

QRectF MonitorGraphicsClusters::boundingRect() const
{
	if (scene())
		return scene()->sceneRect();
	return QRectF();
}


void MonitorGraphicsClusters::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setFont(m_TextFont);


	for (ClusterInfo _info : m_ClustersInfo)
	{
		if (!MonitorConfig::getPointer()->isClustersVisibled() && !_info.isAlert)
			continue;

		painter->setPen(_info.isAlert ? m_AlertPen : m_ActionPen);

		//get text bounding box
		if (_info.isAlert)
		{
			QRectF boundingBox;
			painter->setOpacity(0);
			painter->drawText(QRectF(0, 0, 100, 20), Qt::AlignVCenter, _info.name, &boundingBox);
			painter->setOpacity(1);

			boundingBox.moveTo(_info.rect.x(), _info.rect.y() - boundingBox.height() - 6);

			//draw text bounding box
			QPainterPath path;
			path.addRoundedRect(boundingBox.adjusted(-3, -3, 3, 3), 3, 3);
			painter->setOpacity(0.7);
			painter->fillPath(path, Qt::black);
			painter->setOpacity(1);

			//draw text
			painter->drawText(boundingBox, Qt::AlignVCenter, _info.name);
		}

		//draw rect
		painter->drawRect(_info.rect);
	}
}