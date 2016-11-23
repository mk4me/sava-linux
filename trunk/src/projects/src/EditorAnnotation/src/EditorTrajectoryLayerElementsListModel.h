#pragma once

#include "EditorSingletons.h"
#include "EditorLayerElementsListModel.h"

/**
* \class EditorTrajectoryLayerElementsListModel
*
* \brief Model for EditorTrajectoryLayerElementsList
*
*/
class EditorTrajectoryLayerElementsListModel : public EditorLayerElementsListModel
{
	Q_OBJECT

public:
	EditorTrajectoryLayerElementsListModel(QObject *parent);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role) const override;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

	virtual QModelIndex insertRow() override;
	virtual void removeRow(const QModelIndex& index) override;

private:
	EditorTrajectoryManager* m_TrajectoryManager;
};

