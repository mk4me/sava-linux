#pragma once

#include <QtWidgets/QMenu>
#include <QtCore/QDebug>
#include "EditorAnnotationWidget.h"
#include "EditorSceneObjectLayer.h"

/**
* \class EditorSceneContextMenu
*
* \brief Scene context menu.
*
*/
class EditorSceneContextMenu : public QMenu
{
	Q_OBJECT

public:
	EditorSceneContextMenu(QWidget* parent = 0);
	~EditorSceneContextMenu();

	Q_INVOKABLE void clearSelection();
	Q_INVOKABLE void lockEdit();
	//Q_INVOKABLE void merge();

signals:
	void actionDone();

public slots:
	void onTriggered(QAction* action);
	void onAboutToShow();

private:
	QAction* m_LockEditAction;
	QAction* m_ClearSelectionAction;
	//QAction* m_MergeAction;

	EditorSceneObjectLayer* m_ObjectLayer;
};