#pragma once

#ifndef ANNOTATIONEDITORWINDOW_H
#define ANNOTATIONEDITORWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtCore/QTimer>

#include <utils/Singleton.h>
#include "ui_EditorWindow.h"
#include "EditorSequencesWidget.h"
#include "EditorScene.h"
#include "EditorTimelineWidget.h"
#include "EditorMessageManager.h"
#include "EditorConfigDialog.h"
#include "EditorPopup.h"
#include "EditorSingletons.h"
#include "EditorTimeline.h"
#include "EditorAutoSaveTimer.h"
#include "EditorVideoSaver.h"

#include <vector>


/// <summary>
/// Class EditorWindow.
/// </summary>
/// <seealso cref="QMainWindow" />
/// <seealso cref="EditorMessageListener" />
class EditorWindow : public QMainWindow, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorWindow(QWidget *parent = 0);
	~EditorWindow();


public slots:

	/// <summary>
	/// Load editor configuration.
	/// </summary>
	void load();

	/// <summary>
	/// Saves editor work.
	/// </summary>
	void save();

	/// <summary>
	/// Unselects all objects on scene.
	/// </summary>
	void unselectAll();

	/// <summary>
	/// Shows the configuration window.
	/// </summary>
	void showConfig();

	/// <summary>
	/// Locks the selected scene object to edit its location and size.
	/// </summary>
	void lockEdit();

	/// <summary>
	/// Updates the tool bar widget.
	/// </summary>
	void updateToolBar();

	/// <summary>
	/// Loads the video sequence file from widget dialog.
	/// </summary>
	void loadVideoFromDialog();

private slots:
	void onLoadingStarted();
	void onLoadingFinished();
	void onAboutActionClicked();

private:
	bool loadLastVideo();
	bool loadVideoFromArg(const QString& arg);

	void setGuiEnabled(bool i_Enabled);

	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;
	virtual void keyPressEvent(QKeyEvent *) override;
	virtual void closeEvent(QCloseEvent *) override;


private:
	Ui::EditorWindow ui;

	/// <summary>
	/// The configuration dialog widget.
	/// </summary>
	EditorConfigDialog* m_ConfigDialog;

	/// <summary>
	/// The timeline manager.
	/// </summary>
	EditorTimeline m_TimelineManager;

	//popup object
	EditorPopup* m_Popup;

	/// <summary>
	/// The editor auto save timer
	/// </summary>
	EditorAutoSaveTimer* m_AutoSaveTimer;

	/// <summary>
	/// The video saver
	/// </summary>
	EditorVideoSaver* m_VideoSaver;
};




#endif // ANNOTATIONEDITORWINDOW_H
