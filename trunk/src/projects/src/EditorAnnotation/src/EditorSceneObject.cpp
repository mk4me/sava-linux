#include "EditorSceneObject.h"
#include "EditorScene.h"
#include "EditorGraphicsView.h"
#include "EditorSceneLayerManager.h"


int EditorSceneObject::PEN_WIDTH = 2;
QColor EditorSceneObject::SELECT_COLOR = Qt::red;
QColor EditorSceneObject::UNSELECT_COLOR = Qt::blue;



/*--------------------------------------------------------------------*/
EditorSceneObject::EditorSceneObject(size_t i_Id, QGraphicsItem *parent)
	: QGraphicsRectItem(parent),
	m_Id(i_Id),
	m_IsSelected(false),
	m_ClusterId(-1)
{
	//setAcceptHoverEvents(true);
}

/*--------------------------------------------------------------------*/
EditorSceneObject::~EditorSceneObject()
{
	
}


/*--------------------------------------------------------------------*/
void EditorSceneObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	//jeœli w mamny nie rysowaæ bounding boxow to wyjdz
	if (EditorConfig::getInstance().isSceneObjectsBoundingBoxVisibled())
	{
		painter->setRenderHint(QPainter::Antialiasing, true);

		//draw selected rectangle
		QPen pen;
		pen.setWidth(PEN_WIDTH);
		pen.setColor(m_IsSelected ? SELECT_COLOR : UNSELECT_COLOR);
		painter->setPen(pen);
		painter->drawRect(rect());
	}

	if (EditorConfig::getInstance().isSceneObjectsLabelVisibled())
	{
		//draw label
		drawLabel(painter);
	}
}


/*--------------------------------------------------------------------*/
void EditorSceneObject::drawLabel(QPainter* painter)
{
	QPen pen;
	QColor labelColor = (m_IsSelected ? SELECT_COLOR : UNSELECT_COLOR);
	int labelHeight = 16;
	int labelY = (rect().y() > labelHeight) ? (rect().y() - PEN_WIDTH / 2 - labelHeight) : (rect().y() + rect().height() + PEN_WIDTH / 2);
	QRectF textRect = QRectF(rect().x()-3, labelY, 60, labelHeight);

	painter->setPen(labelColor);
	painter->setBrush(QBrush(labelColor));
	painter->drawRect(textRect);

	//draw text label
	QFont font;
	font.setPixelSize(15);
	font.setBold(true);
	painter->setFont(font);
	painter->setPen(QPen(Qt::white));
	painter->drawText(textRect, QString::number(m_Id), QTextOption(Qt::AlignCenter));
}


/*--------------------------------------------------------------------*/
void EditorSceneObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (!rect().contains(event->pos()))
		return;

	EditorSceneObjectLayer* layer = EditorSceneLayerHelper::getObjectLayer();
	if (layer->getAction() == EditorSceneObjectLayer::EAction::ACTION_NONE)
	{
		if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
			layer->setSelectedObject(this);
	}
	
	QGraphicsRectItem::mousePressEvent(event);
}

/*--------------------------------------------------------------------*/
QRectF EditorSceneObject::boundingRect() const
{
	QRectF updateRec = rect().adjusted(-30, -30, 30, 30);
	return updateRec;
}

/*--------------------------------------------------------------------*/
void EditorSceneObject::updateRegion(){
	scene()->update(boundingRect());
}


