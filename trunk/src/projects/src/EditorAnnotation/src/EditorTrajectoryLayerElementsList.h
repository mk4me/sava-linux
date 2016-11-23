#pragma once

#include "EditorLayerElementsList.h"
#include "EditorTrajectoryLayerElementsListModel.h"
#include "EditorTrajectoryLayerElementsListFilter.h"
#include "EditorTrajectoryLayerElementsListDelegate.h"
#include "EditorLayer.h"

/**
* \class EditorTrajectoryLayerElementsList
*
* \brief View to show list of all trajectories
*
*/
class EditorTrajectoryLayerElementsList : public EditorLayerElementsList
{
	Q_OBJECT

public:
	EditorTrajectoryLayerElementsList(QWidget *parent = 0);

private:
	virtual EditorLayerElementsListModel* getListModel() override { return new EditorTrajectoryLayerElementsListModel(this); }
	virtual EditorLayerElementsListFilter* getListFilter() override { return new EditorTrajectoryLayerElementsListFilter(this); }
	virtual EditorLayerElementsListDelegate* getListDelegate() override { return new EditorTrajectoryLayerElementsListDelegate(this); }

	virtual ELayer getLayerType() override { return TRAJECTORY; }

	virtual int getElementId(const QModelIndex& index) override;

};

