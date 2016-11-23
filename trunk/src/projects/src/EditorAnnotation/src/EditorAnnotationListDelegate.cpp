#include "EditorAnnotationListDelegate.h"
#include "EditorSceneLayerManager.h"

/*--------------------------------------------------------------------*/
EditorAnnotationListDelegate::EditorAnnotationListDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	EditorMessageManager::registerMessage(MESSAGE_SCENE_SELECTION_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_SEQUENCE_LOADED, this);
}

/*--------------------------------------------------------------------*/
EditorAnnotationListDelegate::~EditorAnnotationListDelegate(){
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() == 0)
		return;

	size_t id = index.internalId();
	bool isSelectedIndex = (m_SelectedIds.find(id) != m_SelectedIds.end());
	EditorClusterPtr cluster = EditorSequencesManager::getPointer()->getSequence()->getCluster(id);

	if (cluster && cluster->getSource())
	{
		painter->setOpacity(0.3);

		//draw background
		//QColor borderColor = QColor(255, 255, 255, 150);
		painter->fillRect(option.rect, isSelectedIndex ? QColor(255, 255, 255, 150) : QColor(255, 255, 255, 50));

		//draw cluster time range
		QRect clusterSection = getClusterSection(cluster, option.rect);
		painter->fillRect(clusterSection, isSelectedIndex ? Qt::red : Qt::blue);

		//draw border
		painter->setPen(QPen(Qt::black));
		painter->drawRect(option.rect);

		painter->setOpacity(1);

		painter->drawText(option.rect.adjusted(2, 0, 0, 0), QString::number(cluster->getSource()->getClusterId()), QTextOption(Qt::AlignLeft));

		drawClusterContinuation(cluster, option.rect, painter);
		drawClusterActions(cluster, option.rect, painter);
		//if (isSelectedIndex)
		//	drawClusterKeys(cluster, option.rect, painter);
	}
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListDelegate::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	if (i_Message == MESSAGE_SCENE_SELECTION_CHANGED 
		|| i_Message == MESSAGE_SEQUENCE_LOADED)
		m_SelectedIds = EditorSceneLayerHelper::getObjectLayer()->getSelectedObjectsIds();
}

/*--------------------------------------------------------------------*/
QSize EditorAnnotationListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
	return QSize(1, 15);
}


QRect EditorAnnotationListDelegate::getClusterSection(const EditorClusterPtr& cluster, const QRect& i_PaintRect) const
{
	if (cluster)
	{	
		float frameCount = static_cast<float>(EditorTimeline::getPointer()->getFrameCount() - 1);
		int xStartPos = (cluster->getSource()->getStartFrame() / frameCount) * i_PaintRect.width();
		int xEndPos = (cluster->getSource()->getEndFrame() / frameCount) * i_PaintRect.width();

		return QRect(i_PaintRect.x() + xStartPos, i_PaintRect.y(), xEndPos - xStartPos, i_PaintRect.height());
	}

	return QRect();
}

void EditorAnnotationListDelegate::drawClusterKeys(const EditorClusterPtr& cluster, const QRect& i_Rect, QPainter* _painter) const
{
	if (cluster)
	{
		_painter->save();
		_painter->setPen(Qt::black);
		_painter->setBrush(Qt::black);

		float frameCount = static_cast<float>(EditorTimeline::getPointer()->getFrameCount() - 1);
		sequence::Cluster::FramesPositionsMap frames = cluster->getSource()->getFramesPositions();
		for (auto frame : frames)
		{
			float x = float(frame.first) / frameCount * i_Rect.width();
			float y = i_Rect.y() + i_Rect.height() / 2.f;

			_painter->drawEllipse(QPointF(x, y), 2, 2);
		}

		_painter->restore();
	}
}

void EditorAnnotationListDelegate::drawClusterContinuation(const EditorClusterPtr& cluster, const QRect& _rect, QPainter* painter) const
{
	if (!cluster)
		return;

	if (!cluster->hasPrevContinuation() && !cluster->hasNextContinuation())
		return;

	painter->save();
	painter->setPen(QPen(Qt::white, 2));
	painter->setRenderHint(QPainter::Antialiasing);

	if (cluster->hasPrevContinuation())
	{
		QPainterPath path;
		path.moveTo(_rect.topLeft());
		path.lineTo(QPointF(-7, _rect.y() + _rect.height() / 2));
		path.lineTo(_rect.bottomLeft());
		path.translate(7, 0);

		painter->drawPath(path);
	}

	if (cluster->hasNextContinuation())
	{
		QPainterPath path;
		path.moveTo(_rect.topRight());
		path.lineTo(QPointF(_rect.width() + 7, _rect.y() + _rect.height() / 2));
		path.lineTo(_rect.bottomRight());
		path.translate(-7, 0);

		painter->drawPath(path);
	}

	painter->restore();
}

void EditorAnnotationListDelegate::drawClusterActions(const EditorClusterPtr& cluster, const QRect& rect, QPainter* painter) const
{
	if (!cluster)
		return;

	auto actions = cluster->getActions();

	QFont font;
	font.setPixelSize(9);

	painter->save();
	painter->setPen(QPen(Qt::black, 1));
	painter->setFont(font);
	painter->setRenderHint(QPainter::Antialiasing);

	float frameCount = static_cast<float>(EditorTimeline::getPointer()->getFrameCount() - 1);
	for (auto action : actions)
	{
		float x1 = float(action->getStartFrame() / frameCount * rect.width());
		float x2 = float(action->getEndFrame() / frameCount * rect.width());

		//draw action name
		int actionId = action->getSource()->getActionId();
		std::string actionName = config::Glossary::getInstance().getEditorActionName(actionId);
		QString qActionName(actionName.empty() ? "[no action]" : actionName.c_str());

		painter->drawText(QRectF(x1, rect.y(), x2 - x1, rect.height()), qActionName, QTextOption(Qt::AlignCenter));

		//draw action line
		if (action != actions.front())
			painter->drawLine(x1, rect.y(), x1, rect.y() + rect.height());
	}

	painter->restore();
}

/*--------------------------------------------------------------------*/
//void EditorAnnotationListDelegate::drawClusterPartInfo(size_t _clusterId, const QRect& _rect, QPainter* _painter) const
//{
	//EditorSequence::ClusterInfo info = EditorSequencesManager::getPointer()->getSequence()->getClusterInfo(_clusterId);

	//if (!info.hasPrevPart && !info.hasNextPart)
	//	return;

	//_painter->save();
	//_painter->setPen(QPen(Qt::white, 2));
	//_painter->setRenderHint(QPainter::Antialiasing);

	//if (info.hasPrevPart)
	//{
	//	QPainterPath path;
	//	path.moveTo(_rect.topLeft());
	//	path.lineTo(QPointF(-7, _rect.y() + _rect.height() / 2)); 
	//	path.lineTo(_rect.bottomLeft());
	//	path.translate(7, 0);

	//	_painter->drawPath(path);
	//}

	//if (info.hasNextPart)
	//{
	//	QPainterPath path;
	//	path.moveTo(_rect.topRight());
	//	path.lineTo(QPointF(_rect.width() + 7, _rect.y() + _rect.height() / 2));
	//	path.lineTo(_rect.bottomRight());
	//	path.translate(-7, 0);

	//	_painter->drawPath(path);
	//}

	//_painter->restore();
//}

