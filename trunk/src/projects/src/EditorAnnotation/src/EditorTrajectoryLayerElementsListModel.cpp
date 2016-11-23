#include "EditorTrajectoryLayerElementsListModel.h"

/*-------------------------------------------------------------------*/
EditorTrajectoryLayerElementsListModel::EditorTrajectoryLayerElementsListModel(QObject *parent)
	:EditorLayerElementsListModel(parent)
{
	m_TrajectoryManager = EditorTrajectoryManager::getPointer();
}


/*--------------------------------------------------------------------*/
QVariant EditorTrajectoryLayerElementsListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole)
		return QString("Dave");

	return QVariant();
}

/*--------------------------------------------------------------------*/
int EditorTrajectoryLayerElementsListModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return static_cast<int>(m_TrajectoryManager->getGroupCount());
}

/*--------------------------------------------------------------------*/
QModelIndex EditorTrajectoryLayerElementsListModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex()*/) const
{
	EditorTrajectoryGroupPtr traj = m_TrajectoryManager->getGroupByIndex(row);
	if (traj)
		return createIndex(row, column, traj.get());

	return QModelIndex();
}

/*--------------------------------------------------------------------*/
QModelIndex EditorTrajectoryLayerElementsListModel::insertRow()
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	m_TrajectoryManager->addGroup();
	endInsertRows();

	return index(rowCount() - 1, 0);
}

/*--------------------------------------------------------------------*/
void EditorTrajectoryLayerElementsListModel::removeRow(const QModelIndex& index)
{
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	EditorTrajectoryGroup* traj = static_cast<EditorTrajectoryGroup*>(index.internalPointer());
	if (traj)
		m_TrajectoryManager->removeGroup(traj);
	endRemoveRows();
}

