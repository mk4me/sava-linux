#include "EditorInterestingPeriodWidget.h"
#include "QtWidgets/QGraphicsItem"
#include "EditorInterestingPeriodScene.h"
#include <QtCore/QDebug>
#include "EditorTimeline.h"


/*--------------------------------------------------------------------*/
EditorInterestingPeriodWidget::EditorInterestingPeriodWidget(QWidget *parent)
	: QGraphicsView(parent)
{
	m_InterestingPeriods = new EditorInterestingPeriodScene();

	m_Scene = new QGraphicsScene(this);
	m_Scene->setSceneRect(0, 0, 10, SCENE_HEIGH);
	setScene(m_Scene);

	createLine();

	EditorMessageManager::registerMessage(MESSAGE_FRAME_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_SEQUENCE_LOADED, this);
}

/*--------------------------------------------------------------------*/
EditorInterestingPeriodWidget::~EditorInterestingPeriodWidget()
{
	EditorMessageManager::unregisterMessages(this);

	deleteRects();

	delete m_InterestingPeriods;
	m_InterestingPeriods = nullptr;
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriodWidget::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	switch (i_Message)
	{
	case MESSAGE_FRAME_CHANGED:
		m_LineItem->setPos(EditorTimeline::getInstance().getCurrentFrame(), 0);
		update();
		break;

	case MESSAGE_SEQUENCE_LOADED:
		deleteRects();
		size_t framesNum = EditorSequencesManager::getInstance().getSequence()->getNumFrames();
		m_Scene->setSceneRect(0, 0, framesNum - 1, SCENE_HEIGH);
		m_LineItem->setPos(0, 0);
		m_LineItem->setVisible(true);
		m_InterestingPeriods->update();
		fitInView(m_Scene->sceneRect());
		createRects();
		update();
	}
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriodWidget::createRects()
{
	auto periods = m_InterestingPeriods->getPeriods();
	for (auto it = periods.begin(); it != periods.end(); ++it)
	{
		RectItem * item = new RectItem();
		item->setRect(it->beginFrame, 2, it->duration, m_Scene->height()-4);

		m_Scene->addItem(item);
		m_RectsItems.push_back(item);
	}
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriodWidget::deleteRects()
{
	for (RectItem* item : m_RectsItems)
		m_Scene->removeItem(item);

	qDeleteAll(m_RectsItems);
	m_RectsItems.clear();
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriodWidget::createLine()
{
	QPen pen(Qt::white);
	pen.setWidth(1);

	m_LineItem = new QGraphicsLineItem(0, 0, 0, SCENE_HEIGH);
	m_LineItem->setPen(pen);
	m_LineItem->setTransformOriginPoint(0, 0);
	m_LineItem->setPos(0, 0);
	m_LineItem->setZValue(10);
	m_LineItem->setVisible(false);

	m_Scene->addItem(m_LineItem);
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriodWidget::nextPeriod()
{
	int currentFrame = EditorTimeline::getInstance().getCurrentFrame();
	auto period = m_InterestingPeriods->getNextPeriod(currentFrame);
	EditorTimeline::getInstance().setCurrentFrame(period.beginFrame);
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriodWidget::prevPeriod()
{
	int currentFrame = EditorTimeline::getInstance().getCurrentFrame();
	auto period = m_InterestingPeriods->getPrevPeriod(currentFrame);
	EditorTimeline::getInstance().setCurrentFrame(period.beginFrame);
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriodWidget::resizeEvent(QResizeEvent *)
{
	fitInView(m_Scene->sceneRect());
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriodWidget::showEvent(QShowEvent *)
{
	fitInView(m_Scene->sceneRect());
}



// RECTANGLE SCENE ITEM


/*--------------------------------------------------------------------*/
EditorInterestingPeriodWidget::RectItem::RectItem()
 :m_NormalColor(0, 94, 188),
  m_HoverColor(Qt::darkRed)
{
	setBrush(m_NormalColor);
	setPen(QPen(m_NormalColor, 2));
	setAcceptHoverEvents(true);
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriodWidget::RectItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	setPen(QPen(m_HoverColor, 1));
	setZValue(1);
	QGraphicsRectItem::hoverEnterEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriodWidget::RectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	setPen(QPen(m_NormalColor, 1));
	setZValue(0);
	QGraphicsRectItem::hoverLeaveEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorInterestingPeriodWidget::RectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	EditorTimeline::getInstance().setCurrentFrame(static_cast<int>(rect().x()));
	QGraphicsRectItem::mousePressEvent(event);
}
