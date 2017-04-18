#include "EditorAnnotationListModel.h"
#include "EditorSingleSequence.h"

/*--------------------------------------------------------------------*/
EditorAnnotationListModel::EditorAnnotationListModel(QObject *parent)
	: QAbstractTableModel(parent){
}

/*--------------------------------------------------------------------*/
EditorAnnotationListModel::~EditorAnnotationListModel(){
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListModel::beginReset(){
	QAbstractTableModel::beginResetModel();
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListModel::endReset(){
	QAbstractTableModel::endResetModel();
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
			int clusterId = (*it)->getUniqueId();

			m_CachedModelIndexes.push_back(createIndex(row, 1, clusterId));
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
		return m_CachedModelIndexes.at(row);
	
	return QModelIndex();
}

/*--------------------------------------------------------------------*/
int EditorAnnotationListModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const {
	return m_CachedModelIndexes.size();
}

/*--------------------------------------------------------------------*/
int EditorAnnotationListModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const {
	return 1;
}

/*--------------------------------------------------------------------*/
QVariant EditorAnnotationListModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const {
	return QVariant();
}

QVariant EditorAnnotationListModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Vertical && section < m_CachedModelIndexes.size())
		return QString::number(m_CachedModelIndexes.at(section).internalId());

	return QVariant();
}

/*--------------------------------------------------------------------*/
QModelIndex EditorAnnotationListModel::getIndexByInternalId(int i_Id)
{
	for (auto it : m_CachedModelIndexes)
		if (it.internalId() == i_Id)
			return it;

	return QModelIndex();
}


