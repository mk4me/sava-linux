#pragma once
#ifndef EDITORACTIONWIDGET_H
#define EDITORACTIONWIDGET_H

#include "ui_EditorActionWidget.h"
#include "EditorMessageManager.h"
#include "EditorSingleSequence.h"
#include "EditorSequencesManager.h"
#include "EditorSceneLayerManager.h"
#include "EditorActionScene.h"

#include <QtWidgets/QWidget>

class EditorActionWidget : public QWidget, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorActionWidget(QWidget *parent = 0);
	~EditorActionWidget();

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
	Ui::EditorActionWidget ui;
	EditorActionScene* m_Scene;

	EditorClusterPtr m_CurrentCluster;
	EditorActionPtr m_CurrentAction;

	EditorSequencePtr m_Sequence;
};

#endif // EDITORACTIONWIDGET_H
