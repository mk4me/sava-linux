#include "EditorAnnotationListDelegate.h"
#include "include\EditorSceneLayerManager.h"

/*--------------------------------------------------------------------*/
EditorAnnotationListDelegate::EditorAnnotationListDelegate(QObject *parent)
	: QStyledItemDelegate(parent){
}

/*--------------------------------------------------------------------*/
EditorAnnotationListDelegate::~EditorAnnotationListDelegate(){
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

	size_t clusterId = index.internalId();
	bool isSelectedIndex = (EditorSceneLayerHelper::getObjectLayer()->getSelectedObjectId() == clusterId);

	painter->setOpacity(0.3);

	//draw background
	//QColor borderColor = QColor(255, 255, 255, 150);
	painter->fillRect(option.rect, isSelectedIndex ? QColor(255, 255, 255, 150) : QColor(255, 255, 255, 50));

	//draw cluster time range
	QRect clusterSection = getClusterSection(clusterId, option.rect);
	painter->fillRect(clusterSection, isSelectedIndex ? Qt::red : Qt::blue);

	//draw border
	painter->setPen(QPen(Qt::black));
	painter->drawRect(option.rect);


	painter->setOpacity(1);

	//draw cluster id
	painter->drawText(option.rect.adjusted(10,0,0,0), QString::number(clusterId));

	//draw action text
	drawActionText(clusterId, option.rect.adjusted(50, 0, 0, 0), painter);
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListDelegate::drawActionText(size_t _clusterId, const QRect& _paintRect, QPainter* _painter) const
{
	ActionPtr actionPtr = EditorSequencesManager::getPointer()->getSequence()->getAction(_clusterId);
	if (actionPtr)
	{
		std::vector<std::string> glossaryActions = utils::Glossary::getInstance().getAnnotations();
		int actionId = actionPtr->getActionId();
		if (actionId >= 0 && actionId < glossaryActions.size())
		{
			QString actionName = glossaryActions.at(actionId).c_str();
			_painter->drawText(_paintRect, actionName);
		}
	}
}


/*--------------------------------------------------------------------*/
QSize EditorAnnotationListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
	return QSize(1, 15);
}

/*--------------------------------------------------------------------*/
QRect EditorAnnotationListDelegate::getClusterSection(size_t clusterId, const QRect& i_PaintRect) const
{
	ClusterPtr cluster = EditorSequencesManager::getPointer()->getSequence()->getCluster(clusterId);
	if (cluster)
	{
		float frameCount = static_cast<float>(EditorTimeline::getPointer()->getFrameCount() - 1);

		int xStartPos = (cluster->getStartFrame() / frameCount) * i_PaintRect.width();
		int xEndPos = (cluster->getEndFrame() / frameCount) * i_PaintRect.width();

		return QRect(i_PaintRect.x() + xStartPos, i_PaintRect.y(), xEndPos - xStartPos, i_PaintRect.height());
	}

	return QRect();
}

