#include "EditorTrajectoryLayerElementsListDelegate.h"
#include "QtCore/QDebug"
#include "EditorLayerElementsList.h"
#include "EditorTimeline.h"

/*-------------------------------------------------------------------*/
EditorTrajectoryLayerElementsListDelegate::EditorTrajectoryLayerElementsListDelegate(QObject *parent)
	:EditorLayerElementsListDelegate(parent)
{
	m_ParentList = dynamic_cast<EditorLayerElementsList*>(parent);
}


/*--------------------------------------------------------------------*/
void EditorTrajectoryLayerElementsListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

	QModelIndex mapIndex = m_ParentList->mapToSource(index);

	if (!mapIndex.isValid())
		return;

	painter->save();

	bool isIndexSelected = m_ParentList->currentIndex() == index;
	painter->fillRect(option.rect, isIndexSelected ? Qt::lightGray : Qt::darkGray);

	EditorTrajectoryGroup* group = static_cast<EditorTrajectoryGroup*>(mapIndex.internalPointer());
	if (group)
	{
		painter->setPen(Qt::black);
		painter->setBrush(QBrush(QColor(255, 191, 0)));

		QRect section = getTrajSection(group, option.rect);
		if (section.width() > 0)
		{
			section.adjust(0, 2, 0, -4);
			painter->drawRect(section);
			painter->drawEllipse(QPoint(section.x(), section.y() + section.height() / 2), 2, 2);
			painter->drawEllipse(QPoint(section.x() + section.width(), section.y() + section.height() / 2), 2, 2);
		}

		painter->drawText(option.rect.adjusted(2, 0, 0, 0), QString::number(group->getId()));

		//draw info 
		if (group->getNumPoints() == 0)
			painter->drawText(option.rect, "No points.", QTextOption(Qt::AlignCenter));
	}

	painter->setBrush(QBrush());
	painter->setPen(QColor(0, 0, 0, 0));

	//draw unactive row
	if (!isIndexSelected)
		painter->fillRect(option.rect, QColor(169, 169, 169, 100));
	else
		painter->drawRect(option.rect.adjusted(0, 0, 0, -1));

	painter->restore();
}

/*--------------------------------------------------------------------*/
QRect EditorTrajectoryLayerElementsListDelegate::getTrajSection(const EditorTrajectoryGroup* i_Traj, const QRect& i_PaintRect) const
{
	//REMOVE 
	/*float frameCount = EditorTimeline::getInstance().getFrameCount();
	float startFrame = i_Traj->getStartTime();
	float endFrame = i_Traj->getEndTime();
	int frameOffset = EditorCrumbledStreamData::getInstance().getCrumbledSequence()->RemovedFrameCount();

	int beginXPos = (startFrame - frameOffset) / frameCount * i_PaintRect.width();
	int endXPos = (endFrame - frameOffset) / frameCount * i_PaintRect.width();
	beginXPos = max(0, beginXPos);
	endXPos = min(endXPos, i_PaintRect.width());

	return QRect(i_PaintRect.x() + beginXPos, i_PaintRect.y(), endXPos - beginXPos, i_PaintRect.height());*/
	return QRect();
}