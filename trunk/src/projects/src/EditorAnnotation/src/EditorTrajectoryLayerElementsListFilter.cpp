#include "EditorTrajectoryLayerElementsListFilter.h"
#include "EditorTrajectoryGroup.h"

/*-------------------------------------------------------------------*/
EditorTrajectoryLayerElementsListFilter::EditorTrajectoryLayerElementsListFilter(QObject* parent /*= 0*/)
	:EditorLayerElementsListFilter(parent)
{

}


/*--------------------------------------------------------------------*/
bool EditorTrajectoryLayerElementsListFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	//REMOVE 
/*
	QModelIndex currentIndex = sourceModel()->index(source_row, 0, source_parent);
	EditorTrajectoryGroup* trajectory = static_cast<EditorTrajectoryGroup*>(currentIndex.internalPointer());

	if (trajectory)
	{
		if (trajectory->getNumPoints() == 0)
			return true;

		CCrumbledStream* stream = EditorCrumbledStreamData::getInstance().getCrumbledStream();
		if (stream && stream->Crumbles())
		{
			int removedFrames = stream->Crumbles()->RemovedFrameCount();
			if ((trajectory->getStartTime() - removedFrames >= 0) || (trajectory->getEndTime() - removedFrames >= 0))
				return true;
		}
	}*/

	return false;
}