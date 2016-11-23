#pragma once
#ifndef EDITORLAYERTREEVIEWDELEGATE_H
#define EDITORLAYERTREEVIEWDELEGATE_H

#include <QtWidgets/QStyledItemDelegate>

/**
* \class EditorLayerElementsListDelegate
*
* \brief Base delegate using in EditorLayerElementsList
*
*/
class EditorLayerElementsListDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	EditorLayerElementsListDelegate(QObject *parent);
	virtual ~EditorLayerElementsListDelegate();

};

#endif // EDITORLAYERTREEVIEWDELEGATE_H
