#include "EditorWindow.h"
#include "EditorTrajectoryLoader.h"
#include "EditorConfigDialog.h"
#include "EditorSceneLayerManager.h"
#include "EditorSequencesHelper.h"
#include "EditorSequencesExplorer.h"

#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QFileDialog>
#include "Utils/Filesystem.h"

/*--------------------------------------------------------------------*/
EditorWindow::EditorWindow(QWidget* parent /*=0*/)
: QMainWindow(parent)
{
	//load editor data
	load();

	//init gui
	ui.setupUi(this);
	ui.actionOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui.actionSave->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
	ui.actionUnselectObjects->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
	ui.actionUnselectObjects->setEnabled(false);
	ui.actionLockEdit->setEnabled(false);
	ui.actionShutdown->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
	ui.m_PeriodNextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
	ui.m_PeriodPrevButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));

	tabifyDockWidget(ui.annotationDockWidget, ui.trajectoryDockWidget);
	//setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::East);
	ui.annotationDockWidget->raise();
	ui.trajectoryDockWidget->hide();
	//ui.annotationDockWidget->hide();
	//hide period time
	ui.m_PeriodFrame->hide();

	//init editor objects
	m_AutoSaveTimer = new EditorAutoSaveTimer(this);
	m_ConfigDialog = new EditorConfigDialog(this);
	m_Popup = new EditorPopup(ui.m_EditorView);

	//connections 
	connect(ui.m_EditorView, SIGNAL(updateRequest()), this, SLOT(updateToolBar()));
	connect(m_AutoSaveTimer, SIGNAL(timeout()), this, SLOT(save()));

	//register messages
	EditorMessageManager::registerMessage(MESSAGE_FRAME_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_SELECTION_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_CONFIG_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_NEW_SEQUENCE_LOAD_FINISH, this);

	//try load last files (working dir + sequence)
	loadLastVideo();

	ui.m_EditorView->setFocus(Qt::NoFocusReason);
}

/*--------------------------------------------------------------------*/
EditorWindow::~EditorWindow()
{
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorWindow::load()
{
	//load config
	EditorConfig::getInstance().load();

	//load trajectories
	//EditorTrajectoryLoader::load(EditorTrajectoryManager::getInstance(), EditorConfig::getInstance().getWorkingDir() + "\\trajectories.dat");
}

/*--------------------------------------------------------------------*/
void EditorWindow::save()
{
	//save config
	EditorConfig::getInstance().save();

	//save sequences 
	EditorSequencesManager::getInstance().save();

	//save trajectories
	//EditorTrajectoryLoader::save(EditorTrajectoryManager::getInstance(), EditorConfig::getInstance().getWorkingDir() + "\\trajectories.dat");

	//message to user
	m_Popup->show("Saved");
}


/*--------------------------------------------------------------------*/
void EditorWindow::loadLastVideo()
{
	std::string lastVideoFile = EditorConfig::getInstance().getLastVideoPath();
	if (!lastVideoFile.empty())
		EditorSequencesManager::getPointer()->load(lastVideoFile);
}

/*--------------------------------------------------------------------*/
void EditorWindow::loadVideoFromDialog()
{
	std::string l_LastVideoPath = EditorConfig::getInstance().getLastVideoPath();
	if (l_LastVideoPath.empty())
		l_LastVideoPath = utils::Filesystem::getAppPath();

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setDirectory(l_LastVideoPath.c_str());
	dialog.setNameFilters(QStringList() << EditorSequencesExplorer::SEQUENCE_EXTENSIONS);

	if (dialog.exec() == QDialog::Rejected)
		return;

	QStringList l_Files = dialog.selectedFiles();
	if (!l_Files.empty())
	{
		std::string fileToLoad = QFileInfo(l_Files.at(0)).absoluteFilePath().toStdString();
		EditorSequencesManager::getPointer()->load(fileToLoad);
	}
}

/*--------------------------------------------------------------------*/
void EditorWindow::lockEdit(){
	if (ui.actionLockEdit->isEnabled())
		if (ui.actionLockEdit->isChecked())
			EditorMessageManager::sendMessage(MESSAGE_LOCK_SCENE_OBJECT);
		else
			EditorMessageManager::sendMessage(MESSAGE_UNCLOCK_SCENE_OBJECT);

	updateToolBar();
}


/*--------------------------------------------------------------------*/
void EditorWindow::unselectAll() {
	EditorMessageManager::sendMessage(MESSAGE_SCENE_UNSELECT_ALL);
}

/*--------------------------------------------------------------------*/
void EditorWindow::keyReleaseEvent(QKeyEvent * event)
{
	switch (event->key())
	{
		case Qt::Key_Space:
			m_TimelineManager.tooglePlay();
			break;

		case Qt::Key_D:
		case Qt::Key_Right:
			m_TimelineManager.nextFrame();
			break;

		case Qt::Key_A:
		case Qt::Key_Left:
			m_TimelineManager.prevFrame();
			break;

		case Qt::Key_Up:
			m_TimelineManager.setPlayBackward(false);
			break;

		case Qt::Key_Down:
			m_TimelineManager.setPlayBackward(true);
			break;

		case Qt::Key_Escape:
			shutdown();
			break;
	}
}


/*--------------------------------------------------------------------*/
void EditorWindow::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	switch (i_Message)
	{
		case MESSAGE_FRAME_CHANGED:
			ui.m_EditorView->redraw();
			break;

		case MESSAGE_SCENE_SELECTION_CHANGED:
			updateToolBar();
			break;

		case MESSAGE_CONFIG_CHANGED:
			unselectAll();
			updateToolBar();
			break;

		case MESSAGE_NEW_SEQUENCE_LOAD_FINISH:
			EditorConfig::getInstance().setLastVideoPath(EditorSequencesManager::getInstance().getSequence()->getVideoPath());
			break;
	}
}

/*--------------------------------------------------------------------*/
void EditorWindow::updateToolBar()
{
	bool b_SelectedObject = (EditorSceneLayerHelper::getObjectLayer()->getSelectedObjectId() >= 0);
	bool b_IsLocked = EditorSceneLayerHelper::getObjectLayer()->isEditLocked();
	ui.actionUnselectObjects->setEnabled(b_SelectedObject);
	ui.actionLockEdit->setEnabled(b_SelectedObject);
	ui.actionLockEdit->setChecked(b_IsLocked);
	ui.actionLockEdit->setText(b_IsLocked ? "Locked " : "Lock ");
}

/*--------------------------------------------------------------------*/
void EditorWindow::showConfig(){
	m_ConfigDialog->exec();
}

/*--------------------------------------------------------------------*/
void EditorWindow::shutdown(){
	save();
	QApplication::quit();
}


