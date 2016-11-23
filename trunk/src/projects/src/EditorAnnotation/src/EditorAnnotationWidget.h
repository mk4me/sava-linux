#pragma once
#ifndef EDITORANNOTATIONTREEWIDGET_H
#define EDITORANNOTATIONTREEWIDGET_H

#include <QtWidgets/QWidget>
#include "ui_EditorAnnotationWidget.h"
#include "EditorMessageManager.h"
#include "EditorSceneLayerManager.h"

/**
* \class EditorAnnotationWidget
*
* \brief Widget to managet annotation functionality in Editor
*/
class EditorAnnotationWidget : public QWidget, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorAnnotationWidget(QWidget *parent = 0);
	~EditorAnnotationWidget();

public slots:

	/// <summary>
	/// On the list item clicked.
	/// </summary>
	/// <param name="index">The index of clicked item</param>
	void onListClicked(const QModelIndex& index);

	/// <summary>
	/// Add new list item.
	/// </summary>
	void add();

	/// <summary>
	/// Removes selected list items.
	/// </summary>
	void remove();

	/// <summary>
	/// Change begin range of list item.
	/// </summary>
	void rangeBegin();

	/// <summary>
	/// Change end range of list item.
	/// </summary>
	void rangeEnd();

protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

private:
	void updateGui();

private:
	Ui::EditorAnnotationWidget ui;
	EditorSceneObjectLayer* m_ObjectLayer;
	EditorSequencePtr m_Sequence;
};

#endif // EDITORANNOTATIONTREEWIDGET_H
