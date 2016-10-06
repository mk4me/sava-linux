#pragma once

#include "QtCore\QSortFilterProxyModel"

/**
* \class EditorLayerElementsListFilter
*
* \brief Base filter using in EditorLayerElementsList
*
*/
class EditorLayerElementsListFilter : public QSortFilterProxyModel
{
public:
	EditorLayerElementsListFilter(QObject* parent = 0);
	virtual ~EditorLayerElementsListFilter();

};