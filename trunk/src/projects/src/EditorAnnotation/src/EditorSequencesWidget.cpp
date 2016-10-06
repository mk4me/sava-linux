#include "EditorSequencesWidget.h"
#include "include\EditorConfig.h"


/*--------------------------------------------------------------------*/
EditorSequencesWidget::EditorSequencesWidget(QWidget *parent)
	: QWidget(parent)
	, m_WasPlaying(false)
	, m_NextFrame(-1)
{
	ui.setupUi(this);

	m_CachedVideoExplorer = EditorSequencesManager::getPointer()->getExplorer();
	m_SeqManager = EditorSequencesManager::getPointer();
	m_Timeline = EditorTimeline::getPointer();

	ui.m_AutoReloadCheckBox->setChecked(EditorConfig::getPointer()->isAutoReload());

	EditorMessageManager::registerMessage(MESSAGE_NEW_SEQUENCE_LOAD_FINISH, this);
	EditorMessageManager::registerMessage(MESSAGE_PREPARE_FRAME_CHANGE, this);
}

/*--------------------------------------------------------------------*/
EditorSequencesWidget::~EditorSequencesWidget()
{
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::reload()
{
	ui.fileListWidget->clear();
	ui.fileListWidget->clearSelection();

	std::string l_CurrentVideoPath = m_SeqManager->getSequence()->getVideoPath();
	std::string l_CurrentVideoName = QFileInfo(l_CurrentVideoPath.c_str()).fileName().toStdString();

	std::vector<std::string> l_VideoList = m_CachedVideoExplorer->getSequences();
	for (auto it = l_VideoList.begin(); it != l_VideoList.end(); ++it)
	{
		QListWidgetItem* item = new QListWidgetItem(it->c_str());
		item->setSizeHint(QSize(item->sizeHint().width(), 20));

		ui.fileListWidget->addItem(item);
		item->setSelected(l_CurrentVideoName == *it);
	}
}


/*--------------------------------------------------------------------*/
void EditorSequencesWidget::onItemClicked(QListWidgetItem* item)
{
	if (item)
	{
		std::string l_CurrentVideoPath = m_SeqManager->getSequence()->getVideoPath();
		std::string l_ClickedVideoPath = m_CachedVideoExplorer->getAbsolutePath(item->text().toStdString());
		if (l_CurrentVideoPath != l_ClickedVideoPath) {
			m_SeqManager->load(l_ClickedVideoPath);
		}
	}
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::onAutoReloadChecked()
{
	EditorConfig::getPointer()->setAutoReload(ui.m_AutoReloadCheckBox->isChecked());
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::tryToReload()
{
	bool bLoaded = false;

	if (m_NextFrame >= m_Timeline->getFrameCount()){
		bLoaded = m_SeqManager->loadNext();
	}
	else if (m_NextFrame < 0){
		bLoaded = m_SeqManager->loadPrev();
		if (bLoaded)
			m_Timeline->setCurrentFrame(m_Timeline->getFrameCount() - 1);
	}

	if (bLoaded && m_WasPlaying)
		m_Timeline->play();
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	switch (i_Message)
	{
		case MESSAGE_PREPARE_FRAME_CHANGE:
			if (canAutoReload(i_ExtraParam))
			{
				m_NextFrame = i_ExtraParam;
				m_WasPlaying = m_Timeline->isPlaying();
				QTimer::singleShot(1, this, SLOT(tryToReload()));
			}
			break;

		case MESSAGE_NEW_SEQUENCE_LOAD_FINISH:
			QTimer::singleShot(1, this, SLOT(reload()));
			break;

	}
}

/*--------------------------------------------------------------------*/
bool EditorSequencesWidget::canAutoReload(int nextFrame)
{
	return ui.m_AutoReloadCheckBox->isChecked() 
		&& ((nextFrame >= m_Timeline->getFrameCount()) || (nextFrame < 0));
}


