#include "QtWidgets/QGraphicsPixmapItem"
#include "EditorScene.h"
#include "EditorWindow.h"
#include "EditorSceneLayerManager.h"
#include "EditorSceneObjectLayer.h"
#include "utils/ImageConverter.h"

/*--------------------------------------------------------------------*/
EditorScene::EditorScene(QObject *parent)
: QGraphicsScene(parent)
{
	m_BackgroundItem = new BackgroudGraphicsItem(QPixmap());
	addItem(m_BackgroundItem);

	m_LayerManager = new EditorSceneLayerManager(this);
	m_LayerManager->init();

	EditorMessageManager::registerMessage(MESSAGE_NEW_SEQUENCE_LOAD_FINISH, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_UPDATE, this);

	m_LayerManager->getLayer(OBJECTS)->setFocus();
}

/*--------------------------------------------------------------------*/
EditorScene::~EditorScene()
{
	EditorMessageManager::unregisterMessages(this);

	delete m_LayerManager;
}

/*--------------------------------------------------------------------*/
void EditorScene::reload()
{
	//set scene resolution
	std::shared_ptr<IVideo> video = EditorSequencesManager::getInstance().getSequence()->getVideo();
	if (video->getNumFrames() > 0)
	{
		cv::Mat frame = video->getFrameImage(0);
		setSceneRect(0, 0, frame.size().width, frame.size().height);

		m_LayerManager->reload();
	}
}

/*--------------------------------------------------------------------*/
void EditorScene::redraw()
{
	//get current time
	int currentFrame = EditorTimeline::getInstance().getCurrentFrame();

	//get background image
	cv::Mat cvImage = EditorSequencesManager::getInstance().getSequence()->getVideo()->getFrameImage(currentFrame);

	//draw background
	QPixmap pixmap = utils::image_converter::cvMatToQPixmap(cvImage);
	m_BackgroundItem->setPixmap(pixmap);

	//draw all scene objects
	m_LayerManager->redraw();
}


/*--------------------------------------------------------------------*/
void EditorScene::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	switch (i_Message)
	{
		case MESSAGE_NEW_SEQUENCE_LOAD_FINISH:
			reload();
			redraw();
			break;

		case MESSAGE_SCENE_UPDATE:
			redraw();
			break;
	}
}

/*--------------------------------------------------------------------*/
void EditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	m_LayerManager->mouseMove(event);
	QGraphicsScene::mouseMoveEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	m_LayerManager->mouseRelease(event);
	QGraphicsScene::mouseReleaseEvent(event);
}

void EditorScene::BackgroudGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	QGraphicsPixmapItem::paint(painter, option, widget);
}