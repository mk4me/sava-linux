#include "EditorSceneObject.h"
#include "EditorScene.h"
#include "EditorGraphicsView.h"
#include "EditorSceneLayerManager.h"
#include "EditorSequencesManager.h"


int EditorSceneObject::PEN_WIDTH = 2;
QColor EditorSceneObject::SELECT_COLOR = Qt::red;
QColor EditorSceneObject::UNSELECT_COLOR = Qt::blue;



/*--------------------------------------------------------------------*/
EditorSceneObject::EditorSceneObject(size_t i_Id, QGraphicsItem *parent)
	: QGraphicsRectItem(parent),
	m_Id(i_Id),
	m_IsSelected(false)
{
	//setAcceptHoverEvents(true);
	m_Cluster = EditorSequencesManager::getInstance().getSequence()->getCluster(m_Id);
}



/*--------------------------------------------------------------------*/
void EditorSceneObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	//jeœli w mamny nie rysowaæ bounding boxow to wyjdz
	if (EditorConfig::getInstance().isSceneObjectsBoundingBoxVisibled())
	{
		painter->setRenderHint(QPainter::Antialiasing, true);

		if (EditorConfig::getInstance().isSceneObjectsLabelVisibled())
		{
			//draw label
			drawLabel(painter);
		}

		//draw selected rectangle
		QPen pen;
		pen.setWidth(PEN_WIDTH);
		pen.setColor(m_IsSelected ? SELECT_COLOR : UNSELECT_COLOR);
		painter->setPen(pen);
		painter->drawRect(rect());
	}
}


/*--------------------------------------------------------------------*/
void EditorSceneObject::drawLabel(QPainter* painter)
{
	QColor labelColor = (m_IsSelected ? SELECT_COLOR : UNSELECT_COLOR);
	int labelHeight = 16;
	int labelY = (rect().y() > labelHeight) ? (rect().y() - PEN_WIDTH / 2 - labelHeight) : (rect().y() + rect().height() + PEN_WIDTH / 2);
	QRectF textRect = QRectF(rect().x() - 3, labelY, 500, labelHeight);


	QString labelText;
	if (m_Cluster)
	{
		labelText = QString("  %1").arg(m_Cluster->getSource()->getClusterId());

		EditorActionPtr currentAction = m_Cluster->getActionByTime(EditorTimeline::getInstance().getCurrentFrame());
		if (currentAction)
		{
			int actionId = currentAction->getSource()->getActionId();
			std::string actionName = config::Glossary::getInstance().getEditorActionName(actionId);
			if (!actionName.empty())
				labelText += ":  " + QString(actionName.empty() ? "[no action]" : actionName.c_str());
		}
	}

	QRectF boundingRect;
	QFont font;
	font.setPixelSize(14);
	font.setBold(true);


	painter->save();

	painter->setBrush(QBrush(labelColor));
	painter->setFont(font);
	painter->setPen(QPen(Qt::white));

	painter->setOpacity(0);
	painter->drawText(textRect, Qt::AlignLeft, labelText, &boundingRect);
	painter->setOpacity(1);

	painter->drawRect(boundingRect.adjusted(0, 0, 3, 0));
	painter->drawText(boundingRect, Qt::AlignLeft, labelText);

	painter->restore();
}


/*--------------------------------------------------------------------*/
void EditorSceneObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (!rect().contains(event->pos()))
		return;

	EditorSceneObjectLayer* layer = EditorSceneLayerHelper::getObjectLayer();
	if (layer->getAction() == EditorSceneObjectLayer::EAction::ACTION_NONE)
	{
		if (event->button() == Qt::LeftButton /*|| event->button() == Qt::RightButton */)
			layer->setSelectedObject(this);
	}
	
	QGraphicsRectItem::mousePressEvent(event);
}

/*--------------------------------------------------------------------*/
QRectF EditorSceneObject::boundingRect() const
{
	QRectF updateRec = rect().adjusted(-20, -20, 20, 20);
	return updateRec;
}

/*--------------------------------------------------------------------*/
void EditorSceneObject::updateRegion()
{
	if (scene())
		scene()->update(boundingRect());
}


