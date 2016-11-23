#pragma once
#ifndef EDITORPROPERTYWIDGET_H
#define EDITORPROPERTYWIDGET_H

#include "ui_EditorPropertyWidget.h"
#include "EditorMessageManager.h"
#include "EditorSingleSequence.h"
#include "EditorSequencesManager.h"
#include "EditorSceneLayerManager.h"
#include "EditorPropertyScene.h"

#include <QtWidgets/QWidget>

class EditorPropertyWidget : public QWidget, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorPropertyWidget(QWidget *parent = 0);
	~EditorPropertyWidget();

protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;
	virtual void resizeEvent(QResizeEvent *) override;

public slots:
	void onActionSplitChanged(int index);
	void onActionChanged(int index);
	void onSplitClicked();
	void onDeleteClicked();
	void onBeginClicked();
	void onEndClicked();

private slots:
	void onSceneActionChanged();

private:
	void initAction();
	void initActionSplit();

	void updateClusterGui();
	void clearClusterGui();

	void updateGui();
	void clearGui();

	void block(bool _enabled);


private:
	Ui::EditorPropertyWidget ui;
	EditorPropertyScene* m_Scene;

	EditorClusterPtr m_CurrentCluster;
	EditorActionPtr m_CurrentAction;

	EditorSequencePtr m_Sequence;
};

#endif // EDITORPROPERTYWIDGET_H
