#include "EditorWindow.h"
#include "EditorTrajectoryLoader.h"
#include "EditorConfigDialog.h"
#include "EditorSceneLayerManager.h"
#include "EditorSequencesHelper.h"
#include "EditorSequencesExplorer.h"
#include "EditorCluster.h"
#include "EditorVideoSaver.h"

#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QFileDialog>

#include "utils/Filesystem.h"
#include "utils/Version.h"
#include "utils/ImageConverter.h"
#include "config/Glossary.h"


/*--------------------------------------------------------------------*/
EditorWindow::EditorWindow(QWidget* parent /*=0*/)
	: QMainWindow(parent)
{
	//load editor data
	load();

	//load glossaries
	config::Glossary::getInstance().load();

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
	//ui.annotationDockWidget->raise();
	ui.trajectoryDockWidget->hide();
	//ui.annotationDockWidget->hide();
	//hide period time
	ui.m_PeriodFrame->hide();

	//init editor objects
	m_AutoSaveTimer = new EditorAutoSaveTimer(this);
	m_ConfigDialog = new EditorConfigDialog(this);
	m_Popup = new EditorPopup(ui.m_EditorView);
	m_VideoSaver = new EditorVideoSaver(ui.m_EditorView);

	setGuiEnabled(false);

	//connections 
	connect(ui.actionVideoSave, SIGNAL(triggered()), m_VideoSaver, SLOT(save()));
	connect(ui.m_EditorView, SIGNAL(updateRequest()), this, SLOT(updateToolBar()));
	connect(m_AutoSaveTimer, SIGNAL(timeout()), this, SLOT(save()));
	connect(EditorSequencesManager::getPointer(), SIGNAL(loadingStarted()), this, SLOT(onLoadingStarted()), Qt::DirectConnection);
	connect(EditorSequencesManager::getPointer(), SIGNAL(loadingFinished(bool)), this, SLOT(onLoadingFinished()), Qt::DirectConnection);

	//register messages
	EditorMessageManager::registerMessage(MESSAGE_FRAME_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_SELECTION_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_CONFIG_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_SEQUENCE_LOADED, this); 
	EditorMessageManager::registerMessage(MESSAGE_SCENE_OBJECT_DELETED, this);

	// -->try load video from app argument
	// OR -->try load last files (working dir + sequence)
	QStringList args = qApp->arguments();
	if (args.size() > 1)
		loadVideoFromArg(args.at(1));	
	else
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
bool EditorWindow::loadLastVideo()
{
	std::vector<std::string> lastVideos = EditorConfig::getInstance().getLastVideosPaths();
	if (!lastVideos.empty())
		EditorSequencesManager::getPointer()->load(lastVideos);

	return !lastVideos.empty();
}

/*--------------------------------------------------------------------*/
bool EditorWindow::loadVideoFromArg(const QString& arg)
{
	QDir qDir(arg);
	if (qDir.exists())
	{
		std::vector<std::string> sequences = EditorSequencesExplorer::getSequencesFilesFromDir(arg.toStdString());
		if (!sequences.empty())
		{
			std::string absoluteFilePath = qDir.absoluteFilePath(sequences.front().c_str()).toStdString();
			EditorSequencesManager::getPointer()->load({ absoluteFilePath });
			return true;
		}
	}

	return false;
}

/*--------------------------------------------------------------------*/
void EditorWindow::setGuiEnabled(bool i_Enabled)
{
	ui.timelineWidget->setEnabled(i_Enabled);
	ui.sequencesWidget->setEnabled(i_Enabled);
	ui.propertyDockWidget->setEnabled(i_Enabled);
	ui.annotationWidget->setEnabled(i_Enabled);
	ui.m_EditorView->setEnabled(i_Enabled);
}

/*--------------------------------------------------------------------*/
void EditorWindow::loadVideoFromDialog()
{
	std::vector<std::string> lastVideos = EditorConfig::getInstance().getLastVideosPaths();
	std::string firstVideoPath = lastVideos.empty() ? utils::Filesystem::getAppPath() : *lastVideos.begin();

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setDirectory(firstVideoPath.c_str());
	dialog.setNameFilters(QStringList() << EditorSequencesExplorer::SEQUENCE_EXTENSIONS);

	if (dialog.exec() == QDialog::Rejected)
		return;

	QStringList l_Files = dialog.selectedFiles();
	if (!l_Files.empty())
	{
		std::string fileToLoad = QFileInfo(l_Files.at(0)).absoluteFilePath().toStdString();
		EditorSequencesManager::getPointer()->load({ fileToLoad });

		//save config
		//EditorConfig::getInstance().save();
	}
}

/*--------------------------------------------------------------------*/
void EditorWindow::onLoadingStarted(){
}

/*--------------------------------------------------------------------*/
void EditorWindow::onLoadingFinished() {
	EditorMessageManager::sendMessage(MESSAGE_SEQUENCE_LOADED);
}

/*--------------------------------------------------------------------*/
void EditorWindow::onAboutActionClicked(){

	std::stringstream ss;
	ss << "Annotation Editor \n\n";
	ss << "Version: " << utils::Version::getFullVersion();
	QMessageBox::about(this, "About", ss.str().c_str());
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

	if (EditorSequencesManager::getInstance().isLoaded())
		EditorMessageManager::sendMessage(MESSAGE_SCENE_UNSELECT_ALL);
}


/*--------------------------------------------------------------------*/
void EditorWindow::keyPressEvent(QKeyEvent * event)
{
	bool seqLoaded = EditorSequencesManager::getInstance().isLoaded();

	switch (event->key())
	{
		case Qt::Key_Space:
			if (seqLoaded) m_TimelineManager.tooglePlay();
			break;

		case Qt::Key_Right:
			if (seqLoaded) m_TimelineManager.nextFrame();
			break;

		case Qt::Key_Left:
			if (seqLoaded) m_TimelineManager.prevFrame();
			break;

		case Qt::Key_Up:
			if (seqLoaded) EditorSequencesManager::getInstance().loadPrev();
			break;

		case Qt::Key_Down:
			if (seqLoaded) EditorSequencesManager::getInstance().loadNext();
			break;


		case Qt::Key_A:
			if (seqLoaded) m_TimelineManager.skipBackward();
			break;

		case Qt::Key_Z:
			if (seqLoaded) m_TimelineManager.skipForward();
			break;

		case Qt::Key_X:
			if (seqLoaded) ui.annotationWidget->rangeEnd();
			break;
	}
}

void EditorWindow::closeEvent(QCloseEvent *e)
{
	save();
	QMainWindow::closeEvent(e);
}

/*--------------------------------------------------------------------*/
void EditorWindow::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	switch (i_Message)
	{
		case MESSAGE_FRAME_CHANGED:
			ui.m_EditorView->redraw();
			break;

		case MESSAGE_SCENE_OBJECT_DELETED:
			updateToolBar();
			break;
		
		case MESSAGE_SCENE_SELECTION_CHANGED:
			updateToolBar();
			break;

		case MESSAGE_CONFIG_CHANGED:
			unselectAll();
			updateToolBar();
			break;

		case MESSAGE_SEQUENCE_LOADED:
			EditorConfig::getInstance().setLastVideosPaths(EditorSequencesManager::getInstance().getVideosPaths());
			setGuiEnabled(true);
			break;
	}
}

/*--------------------------------------------------------------------*/
void EditorWindow::updateToolBar()
{
	auto selectIds = EditorSceneLayerHelper::getObjectLayer()->getSelectedObjectsIds();
	bool b_IsLocked = EditorSceneLayerHelper::getObjectLayer()->isEditLocked();
	ui.actionUnselectObjects->setEnabled(!selectIds.empty());
	ui.actionLockEdit->setEnabled(selectIds.size() == 1);
	ui.actionLockEdit->setChecked(b_IsLocked);
	ui.actionLockEdit->setText(b_IsLocked ? "Locked " : "Lock ");
}

/*--------------------------------------------------------------------*/
void EditorWindow::showConfig(){
	m_ConfigDialog->exec();
}
