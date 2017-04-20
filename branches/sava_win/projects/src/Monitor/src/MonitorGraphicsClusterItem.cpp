#include "MonitorGraphicsClusterItem.h"
#include "MonitorFrames.h"


MonitorGraphicsClusterItem::MonitorGraphicsClusterItem(QGraphicsItem *parent /*=nullptr*/)
	: QGraphicsItem(parent)
	, m_Id(-1)
	, m_IsModified(false)
	, m_ActionName("")
	, m_ShowAlert(false)
	, m_ShowArea(false)
{
	if (s_ActionManager == nullptr)
		s_ActionManager = MonitorActionManager::getPointer();

	if (s_Config == nullptr)
		s_Config = MonitorConfig::getPointer();

	if (s_RegionManager == nullptr)
		s_RegionManager = MonitorRegionsManager::getPointer();
}

void MonitorGraphicsClusterItem::init(const MonitorVideoManager::ActionPair& _source)
{
	m_Source = _source;
	m_Id = m_Source.first->getClusterId();

	if (m_Source.second != nullptr)
	{
		int actionId = m_Source.second->getActionId();
		m_ActionName = QString(s_ActionManager->getActionName(actionId).c_str()).toUpper();
		m_ShowAlert = s_ActionManager->isAlert(actionId);
	}
	else
	{
		m_ActionName = "";
		m_ShowAlert = false;
	}
}


void MonitorGraphicsClusterItem::update(size_t _frame)
{
	cv::Rect rect = m_Source.first->getFrameAt(_frame);
	m_Rect = QRectF(rect.x, rect.y, rect.width, rect.height);

	bool isCollidedWithMask = s_RegionManager->isCollidedWith(MonitorRegion::MASK, m_Rect);

	m_ShowArea = s_Config->isClustersVisibled() &&
		_frame >= m_Source.first->getStartFrame() &&
		_frame <= m_Source.first->getEndFrame();

	setVisible((m_ShowArea || m_ShowAlert) && (!isCollidedWithMask));
}

QRectF MonitorGraphicsClusterItem::boundingRect() const
{
	return QRectF();
}

MonitorActionManager* MonitorGraphicsClusterItem::s_ActionManager = nullptr;

MonitorConfig* MonitorGraphicsClusterItem::s_Config = nullptr;

MonitorRegionsManager* MonitorGraphicsClusterItem::s_RegionManager = nullptr;
