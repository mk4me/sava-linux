#include "ReplayGraphicsScene.h"
#include "ReplayWindow.h"


ReplayGraphicsScene::ReplayGraphicsScene(QObject* parent)
	:QGraphicsScene(parent)
{
	m_ParentWindow = dynamic_cast<ReplayWindow*>(parent);

	setSceneRect(QRectF(0, 0, 100, 100));

	addItem(&m_GraphicsBackground);
	addItem(&m_GraphicsAlert);
	addItem(&m_GraphicsTime);
}


ReplayGraphicsScene::~ReplayGraphicsScene()
{
}

void ReplayGraphicsScene::init()
{
	m_GraphicsTime.init();
}

void ReplayGraphicsScene::reload()
{
	//get alert video
	auto& video = m_ParentWindow->getVideo();
	auto& alert = m_ParentWindow->getAlert();

	//set scene size
	cv::Size _sceneSize = video->getFrameImage(0).size();
	setSceneRect(QRectF(0, 0, _sceneSize.width, _sceneSize.height));

	//set graphics params
	m_GraphicsBackground.setVideo(video);
	m_GraphicsAlert.setAlert(alert);
	m_GraphicsTime.setVideo(video);
}

void ReplayGraphicsScene::update(size_t _frame)
{
	m_GraphicsBackground.update(_frame);
	m_GraphicsAlert.update(_frame);
	m_GraphicsTime.update(_frame);
	QGraphicsScene::update();
}

