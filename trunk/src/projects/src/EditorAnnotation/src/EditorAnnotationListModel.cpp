#include "EditorAnnotationListModel.h"
#include "EditorSequence.h"

/*--------------------------------------------------------------------*/
EditorAnnotationListModel::EditorAnnotationListModel(QObject *parent)
	: QAbstractListModel(parent){
}

/*--------------------------------------------------------------------*/
EditorAnnotationListModel::~EditorAnnotationListModel(){
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListModel::beginReset(){
	QAbstractListModel::beginResetModel();
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListModel::endReset(){
	QAbstractListModel::endResetModel();
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListModel::update()
{
	m_CachedModelIndexes.clear();

	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	if (sequence)
	{
		auto clusterMap = sequence->getClusters();
		for (int row = 0; row < clusterMap.size(); row++)
		{
			auto it = clusterMap.begin();
			std::advance(it, row);
			size_t clusterId = it->first;

			m_CachedModelIndexes.push_back(createIndex(row, 0, clusterId));
		}
	}
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListModel::refresh()
{
	int num = rowCount();
	if (num > 0)
		emit dataChanged(index(0, 0), index(num - 1, 0));
}

/*--------------------------------------------------------------------*/
QModelIndex EditorAnnotationListModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex()*/) const
{
	if (row < m_CachedModelIndexes.size())
		return m_CachedModelIndexes[row];

	return QModelIndex();
}

/*--------------------------------------------------------------------*/
int EditorAnnotationListModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return m_CachedModelIndexes.size();
}

/*--------------------------------------------------------------------*/
QVariant EditorAnnotationListModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	return QVariant();
}

/*--------------------------------------------------------------------*/
QModelIndex EditorAnnotationListModel::getIndexByInternalId(int i_InternalId)
{
	EditorSequencePtr sequence = EditorSequencesManager::getInstance().getSequence();
	if (sequence)
	{
		auto clusterMap = sequence->getClusters();
		auto foundIt = clusterMap.find(i_InternalId);
		if (foundIt != clusterMap.end())
		{
			int row = std::distance(clusterMap.begin(), foundIt);
			if (row < m_CachedModelIndexes.size())
				return m_CachedModelIndexes[row];
		}
	}

	return QModelIndex();
}

