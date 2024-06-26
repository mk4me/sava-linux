#pragma once
#ifndef EDITORCONFIGDIALOG_H
#define EDITORCONFIGDIALOG_H

#include "EditorSingletons.h"
#include <QtWidgets/QDialog>
#include "ui_EditorConfigDialog.h"
#include "QtCore/QDebug"


/// <summary>
/// Kontrolka do wywietlania konfiguracji edytora. 
/// </summary>
class EditorConfigDialog : public QDialog
{
	Q_OBJECT

public:
	EditorConfigDialog(QWidget *parent = 0);
	~EditorConfigDialog();

private slots:
	void onShowObjectsChanged(int state);
	void onShowObjectsBoundingBoxesChanged(int state);
	void onShowObjectsLabelsChanged(int state);
	void onAutoSaveEnabledChanged();
	void onAutoSaveIntervalChanged(int value);
	void onSequenceMultiSelectionChanged(int state);

private:
	Ui::EditorConfigDialog ui;

	EditorConfig* m_Config;
};

#endif // EDITORCONFIGDIALOG_H
