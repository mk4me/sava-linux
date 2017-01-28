#include "EditorActionScene.h"
#include "EditorSequencesManager.h"
#include "EditorTimeline.h"
#include "config/Glossary.h"

#include "QtWidgets/QStyleOptionGraphicsItem"
#include "QtWidgets/QApplication"

/**************************************** GRAPHICS SCENE *****************************/
EditorActionScene::EditorActionScene(QGraphicsView* view, QObject *parent)
	: QGraphicsScene(parent)
	, m_View(view)
	, m_Cluster(nullptr)
	, m_Action(nullptr)
{
	m_View->setScene(this);
	m_View->setBackgroundBrush(QBrush(Qt::black));

	EditorMessageManager::registerMessage(MESSAGE_FRAME_CHANGED, this);

	connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

EditorActionScene::~EditorActionScene(){
}

void EditorActionScene::refresh(const EditorActionPtr& selectedAction /*= nullptr*/)
{
	//clear scene
	clear();
	m_Action.reset();

	if (m_Cluster)
	{
		//set scene rect
		setSceneRect(0, 0, m_View->width()-1, m_View->height()-1);

		qreal lastFrame = EditorSequencesManager::getInstance().getSequence()->getNumFrames() - 1;

		//set actions
		std::vector<EditorActionPtr> actions = m_Cluster->getActions();
		for (auto action : actions)
		{
			int x1 = (action->getStartFrame() / lastFrame) * width();
			int x2 = (action->getEndFrame() / lastFrame) * width();

			GraphicsAction* item = new GraphicsAction();
			QRectF rect = QRectF(x1, 4, x2 - x1, height()-8);
			item->setRect(rect);
			item->setData(Qt::UserRole, QVariant::fromValue<EditorActionPtr>(action));

			addItem(item);

			if (action == selectedAction)
				item->setSelected(true);

			if (selectedAction == nullptr && action == actions.back())
				item->setSelected(true);
		}
		
	}

	QGraphicsScene::update();
}


void EditorActionScene::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/) {
	QGraphicsScene::update();
}
 

void EditorActionScene::drawForeground(QPainter *painter, const QRectF &rect)
{
	painter->setPen(QColor(242, 145, 0));
	painter->setOpacity(0.8);

	float x = EditorTimeline::getInstance().getProgress() * width();

	QPointF p1(x, 0);
	QPointF p2(x, height());
	painter->drawLine(p1, p2);

	QGraphicsScene::drawBackground(painter, rect);
}


void EditorActionScene::onSelectionChanged()
{
	EditorAction* selectedAction = nullptr;
	auto selected = selectedItems();
	if (!selected.empty())
	{
		GraphicsAction* actionItem = dynamic_cast<GraphicsAction*>(selected.front());
		m_Action = actionItem->data(Qt::UserRole).value<EditorActionPtr>();
		emit actionChanged();
	}
}

void EditorActionScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
		return;
	QGraphicsScene::mouseDoubleClickEvent(event);
}

void EditorActionScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
		return;
	QGraphicsScene::mousePressEvent(event);
}

/**************************************** GRAPHICS ACTION *****************************/
EditorActionScene::GraphicsAction::GraphicsAction(QGraphicsItem* parent /*= 0*/)
	:QGraphicsRectItem(parent)
{
	setBrush(QBrush(QColor(137,0,0)));
	setPen(QPen(Qt::white));

	setFlag(QGraphicsItem::ItemIsFocusable);
	setFlag(QGraphicsItem::ItemIsSelectable);
}

void EditorActionScene::GraphicsAction::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	
	if (!isSelected())
		painter->setOpacity(0.6);

	painter->fillRect(option->rect, brush());
	painter->drawRect(option->rect);

	EditorActionPtr action = data(Qt::UserRole).value<EditorActionPtr>();
	if (action)
	{
		QFont font;
		font.setPixelSize(10);
		//font.setItalic(true);
		font.setBold(true);

		painter->setFont(font);
		painter->setPen(QColor(220, 220, 220));

		int actionId = action->getSource()->getActionId();
		std::string actionName = config::Glossary::getInstance().getEditorActionName(actionId);
		if (actionName.empty())
		{
			if (actionId >= 0)
				actionName = "<id: " + std::to_string(actionId) + '>';
			else
				actionName = "[no action]";
		}
				

		painter->drawText(rect(), Qt::AlignCenter , actionName.c_str());
	}
}
