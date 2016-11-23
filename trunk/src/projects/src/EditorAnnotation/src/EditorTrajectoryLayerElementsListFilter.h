#pragma once

#include "EditorSingletons.h"
#include "EditorLayerElementsListFilter.h"

/**
* \class EditorTrajectoryLayerElementsListFilter
*
* \brief Filter for EditorTrajectoryLayerElementsList
*
*/
class EditorTrajectoryLayerElementsListFilter : public EditorLayerElementsListFilter
{
public:
	EditorTrajectoryLayerElementsListFilter(QObject* parent = 0);

protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

};