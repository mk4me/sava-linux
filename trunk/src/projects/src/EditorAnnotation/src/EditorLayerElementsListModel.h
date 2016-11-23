#pragma once
#ifndef EDITORLAYERTREEVIEWMODEL_H
#define EDITORLAYERTREEVIEWMODEL_H

#include <QtCore/QAbstractListModel>

/**
* \class EditorLayerElementsListModel
*
* \brief Base mnodel using in EditorLayerElementsList
*
*/
class EditorLayerElementsListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	EditorLayerElementsListModel(QObject *parent);
	virtual ~EditorLayerElementsListModel();

	/** \brief must ovveride when insert new row*/
	virtual QModelIndex insertRow() = 0;
	/** \brief must ovveride when remove model index*/
	virtual void removeRow(const QModelIndex& index) = 0;
	
};

#endif // EDITORLAYERTREEVIEWMODEL_H
