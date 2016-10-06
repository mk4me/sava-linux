#ifndef EDITORPROPERTYWIDGET_H
#define EDITORPROPERTYWIDGET_H

#include "ui_EditorPropertyWidget.h"
#include "EditorMessageManager.h"
#include "EditorSequence.h"
#include "EditorSequencesManager.h"
#include "EditorSceneLayerManager.h"

#include <QtWidgets\QWidget>

class EditorPropertyWidget : public QWidget, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorPropertyWidget(QWidget *parent = 0);
	~EditorPropertyWidget();

protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

public slots:
	void onActionSplitChanged(int index);
	void onActionChanged(int index);

private:
	void initAction();
	void initActionSplit();

	void updateGui();
	void clearGui();

	void block(bool _enabled);

private:
	Ui::EditorPropertyWidget ui;
};

#endif // EDITORPROPERTYWIDGET_H
