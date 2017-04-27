#include "MonitorGraphicsScene.h"
#include "MonitorRegionsManager.h"


MonitorGraphicsScene::MonitorGraphicsScene(QObject *parent)
	: QGraphicsScene(parent)
	, m_State(NONE)
	, m_IsVideoVisible(false)
{
	m_CachedVideoManger = MonitorVideoManager::getPointer();

	m_AlertGroupRegion = new MonitorGraphicsRegionGroup(MonitorRegion::ALERT);
	m_MaskGroupRegion = new MonitorGraphicsRegionGroup(MonitorRegion::MASK);

	setSceneRect(QRectF(0, 0, 1921, 1200));

	addItem(&m_BackgroundImage);
	addItem(&m_ClustersItem);
	addItem(m_AlertGroupRegion);
	addItem(m_MaskGroupRegion);
	addItem(&m_TimeItem);

#if MONITOR_DEBUG
	addItem(&m_DebugItem);
#endif

	addItem(&m_WaitItem);

	setVideoVisible(false);
}

MonitorGraphicsScene::~MonitorGraphicsScene(){

} 


void MonitorGraphicsScene::init()
{
	assert(m_CachedVideoManger->getMetaVideo()->getVideo());

	cv::Size _sceneSize = m_CachedVideoManger->getMetaVideo()->getVideo()->getFrameImage(0).size();
	setSceneRect(QRectF(0, 0, _sceneSize.width, _sceneSize.height));

	m_AlertGroupRegion->init();
	m_MaskGroupRegion->init();
	m_TimeItem.init();

#if MONITOR_DEBUG
	m_DebugItem.init();
#endif

	m_ClustersItem.init();
}


void MonitorGraphicsScene::setVideoVisible(bool i_Visible)
{
	m_IsVideoVisible = i_Visible;

	m_BackgroundImage.setVisible(i_Visible);
	m_ClustersItem.setVisible(i_Visible);
	m_AlertGroupRegion->setVisible(i_Visible);
	m_MaskGroupRegion->setVisible(i_Visible);
	m_WaitItem.setVisible(!i_Visible);
}


void MonitorGraphicsScene::onVideoPreload()
{

}

void MonitorGraphicsScene::onVideoLoaded()
{
	m_ClustersItem.onVideoLoaded();

	if (!m_IsVideoVisible)
		setVideoVisible(true);
}


void MonitorGraphicsScene::update(size_t _frame)
{
	//update graphics objects
	m_BackgroundImage.update(_frame);
	m_TimeItem.update(_frame);
	m_ClustersItem.update(_frame);

#if MONITOR_DEBUG
	m_DebugItem.update(_frame);
#endif

	QGraphicsScene::update();
}

void MonitorGraphicsScene::setState(EState _nextState)
{
	if (m_State != _nextState) {	
		m_State = _nextState;

		switch (m_State)
		{
		case NONE:
			m_AlertGroupRegion->setState(MonitorGraphicsRegionGroup::NONE);
			m_MaskGroupRegion->setState(MonitorGraphicsRegionGroup::NONE);
			break;

		case ALERT_REGION_EDITABLE:
			m_AlertGroupRegion->setState(MonitorGraphicsRegionGroup::CREATING_NEW_REGION);
			break;

		case MASK_REGION_EDITABLE:
			m_MaskGroupRegion->setState(MonitorGraphicsRegionGroup::CREATING_NEW_REGION);
			break;
		}

		QGraphicsView* view = views().front();
		view->setMouseTracking(m_State != NONE);

		QGraphicsScene::update();
	}
}

void MonitorGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_State == ALERT_REGION_EDITABLE)
		m_AlertGroupRegion->onMouseTracking(event);
	else if(m_State == MASK_REGION_EDITABLE)
		m_MaskGroupRegion->onMouseTracking(event);

	QGraphicsScene::mouseMoveEvent(event);
}


void MonitorGraphicsScene::keyPressEvent(QKeyEvent *event)
{
	if (m_State == ALERT_REGION_EDITABLE)
		m_AlertGroupRegion->onKeyPressEvent(event);
	else if (m_State == MASK_REGION_EDITABLE)
		m_MaskGroupRegion->onKeyPressEvent(event);

	QGraphicsScene::keyPressEvent(event);
}


void MonitorGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_State == ALERT_REGION_EDITABLE)
		m_AlertGroupRegion->onMousePressEvent(event);
	else if (m_State == MASK_REGION_EDITABLE)
		m_MaskGroupRegion->onMousePressEvent(event);
}
