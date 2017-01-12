#pragma once
#ifndef EDITORANNOTATIONLISTMODEL_H
#define EDITORANNOTATIONLISTMODEL_H

#include "EditorSingletons.h"

#include <QtCore/QAbstractTableModel>
#include <vector>


/// <summary>
/// Implementuje model listy w panelu adnotacji.
/// </summary>
class EditorAnnotationListModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	EditorAnnotationListModel(QObject *parent);
	~EditorAnnotationListModel();

	/// <summary>
	/// Begins to reset this model.
	/// </summary>
	void beginReset();

	/// <summary>
	/// Ends to reset this model.
	/// </summary>
	void endReset();

	/// <summary>
	/// Updates this model.
	/// </summary>

	void update();

	/// <summary>
	/// Refresh this model (data has changed).
	/// </summary>
	void refresh();


	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/// <summary>
	/// Gets the model index by object scene id (internal identifier).
	/// </summary>
	/// <param name="i_InternalId">The internal identifier.</param>
	/// <returns>QModelIndex.</returns>
	QModelIndex getIndexByInternalId(int i_InternalId);


private:
	/// <summary>
	/// Cached model indexes. It helps with optimization.
	/// </summary>
	std::vector<QModelIndex> m_CachedModelIndexes;
	
};

#endif // EDITORANNOTATIONLISTMODEL_H
