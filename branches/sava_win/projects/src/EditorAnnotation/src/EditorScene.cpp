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

	EditorMessageManager::registerMessage(MESSAGE_SEQUENCE_LOADED, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_OBJECT_UPDATE, this);
	EditorMessageManager::registerMessage(MESSAGE_COMPLEX_ACTION_UPDATED, this);
	EditorMessageManager::registerMessage(MESSAGE_ACTION_CHANGED, this);

	m_LayerManager->getLayer(OBJECTS)->setFocus();

	setSceneRect(0, 0, 1921, 1081);
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
	EditorSequencePtr seq = EditorSequencesManager::getInstance().getSequence();
	if (seq->getNumFrames() > 0)
	{
		cv::Mat frame = seq->getFrameImage(0);
		setSceneRect(0, 0, frame.size().width, frame.size().height);

		m_LayerManager->reload();
	}
}

/*--------------------------------------------------------------------*/
void EditorScene::redraw(int frameNr /*= -1*/)
{
	//get current time
	int currentFrame = frameNr;
	if (frameNr < 0)
		currentFrame = EditorTimeline::getInstance().getCurrentFrame();

	//get background image
 	cv::Mat cvImage = EditorSequencesManager::getInstance().getSequence()->getFrameImage(currentFrame);

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
		case MESSAGE_SEQUENCE_LOADED:
			reload();
			redraw();
			break;

		case MESSAGE_SCENE_OBJECT_UPDATE:
		case MESSAGE_COMPLEX_ACTION_UPDATED:
		case MESSAGE_ACTION_CHANGED:
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