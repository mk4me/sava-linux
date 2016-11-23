#include "EditorTrajectoryLayerElementsList.h"
#include "EditorTrajectoryLayerElementsListModel.h"

/*-------------------------------------------------------------------*/
EditorTrajectoryLayerElementsList::EditorTrajectoryLayerElementsList(QWidget *parent /*= 0*/)
	:EditorLayerElementsList(parent)
{
	EditorLayerElementsList::init();
}

/*-------------------------------------------------------------------*/
int EditorTrajectoryLayerElementsList::getElementId(const QModelIndex& index)
{
	QModelIndex mappedIndex = mapToSource(index);
	if (mappedIndex.isValid())
	{
		EditorTrajectoryGroup* group = static_cast<EditorTrajectoryGroup*>(mappedIndex.internalPointer());
		if (group)
			return group->getId();
	}

	return -1;
}

