#include "EditorSequencesWidget.h"
#include "EditorConfig.h"


/*--------------------------------------------------------------------*/
EditorSequencesWidget::EditorSequencesWidget(QWidget *parent)
	: QWidget(parent)
	//, m_WasPlaying(false)
	, m_NextFrame(-1)
	, m_CurrenttWorkingDir("")
{
	ui.setupUi(this);

	m_CachedVideoExplorer = EditorSequencesManager::getPointer()->getExplorer();
	m_SeqManager = EditorSequencesManager::getPointer();
	m_Timeline = EditorTimeline::getPointer();

	ui.autoReloadCheckBox->setChecked(EditorConfig::getPointer()->isAutoReload());
	ui.fileListWidget->setSelectionMode(EditorConfig::getPointer()->isSequenceMultiSelection() ? QAbstractItemView::ExtendedSelection : QAbstractItemView::SingleSelection);

	EditorMessageManager::registerMessage(MESSAGE_SEQUENCE_LOADED, this);
	EditorMessageManager::registerMessage(MESSAGE_PREPARE_FRAME_CHANGE, this);
	EditorMessageManager::registerMessage(MESSAGE_FRAME_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_CONFIG_CHANGED, this);

	connect(m_SeqManager, SIGNAL(loadingStarted()), this, SLOT(onLoadingStarted()), Qt::DirectConnection);
	connect(m_SeqManager, SIGNAL(loadingFinished(bool)), this, SLOT(onLoadingFinished()), Qt::DirectConnection);
}

/*--------------------------------------------------------------------*/
EditorSequencesWidget::~EditorSequencesWidget()
{
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::reload()
{
	if (m_CurrenttWorkingDir != m_CachedVideoExplorer->getWorkingDir()) 
	{
		m_CurrenttWorkingDir = m_CachedVideoExplorer->getWorkingDir();
		ui.fileListWidget->clear();
		m_ItemsMap.clear();

		std::vector<std::string> l_VideoList = m_CachedVideoExplorer->getSequences();
		for (auto it = l_VideoList.begin(); it != l_VideoList.end(); ++it)
		{
			std::string seqName = *it;


#if VIDEOS_TIMESTAMPS_ENABLED
			sequence::IVideo::Timestamp seqBeginTime = m_SeqManager->getVideoFirstTime(*it);
			sequence::IVideo::Timestamp seqEndTime = m_SeqManager->getVideoLastTime(*it);
			QString itemText = QString("%1\t[%2 - %3]").arg(seqName.c_str()).arg(timeToString(seqBeginTime).c_str()).arg(timeToString(seqEndTime).c_str());
#else
			QString itemText = seqName.c_str();
#endif


			QListWidgetItem* item = new QListWidgetItem();

			item->setText(itemText);
			item->setData(Qt::UserRole, seqName.c_str());
			item->setSizeHint(QSize(item->sizeHint().width(), 20));

			ui.fileListWidget->addItem(item);
			m_ItemsMap[seqName] = item;
		}
	}

	selectItems();
	scrollToSelectedItem();
}


/*--------------------------------------------------------------------*/
void EditorSequencesWidget::onLoadingStarted()
{
	
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::selectItems()
{
	ui.fileListWidget->clearSelection();

	std::vector<std::string> videosPaths = m_SeqManager->getVideosPaths();
	for (int i = 0; i < videosPaths.size(); i++)
	{
		std::string relativePath = m_CachedVideoExplorer->getRelativePath(videosPaths.at(i));

		auto foundIt = m_ItemsMap.find(relativePath);
		if (foundIt != m_ItemsMap.end())
			foundIt->second->setSelected(true);
	}

	ui.fileListWidget->refresh();
}

/*--------------------------------------------------------------------*/
std::string EditorSequencesWidget::timeToString(const sequence::IVideo::Timestamp& time)
{
	boost::posix_time::time_facet* time_format = new boost::posix_time::time_facet("%H:%M:%S%F");

	std::stringstream ss;
	ss.imbue(std::locale(std::locale::classic(), time_format));
	ss << time;

	std::string s = ss.str().c_str();
	s.erase(s.length() - 7, 7);

	return s;
}


/*--------------------------------------------------------------------*/
void EditorSequencesWidget::scrollToSelectedItem()
{
	QList<QListWidgetItem*> selectedItems = ui.fileListWidget->selectedItems();

	if (selectedItems.isEmpty())
		return;

	QListWidgetItem* centerItem = selectedItems.at(selectedItems.size() / 2);
	ui.fileListWidget->scrollToItem(centerItem);
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::onPrevButtonClicked() {
	EditorSequencesManager::getInstance().loadPrev();
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::onNextButtonClicked() {
	EditorSequencesManager::getInstance().loadNext();
}


/*--------------------------------------------------------------------*/
void EditorSequencesWidget::onListMouseReleased() {
	loadSelectedSequences();
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::loadSelectedSequences()
{
	
	QList<QListWidgetItem*> items = ui.fileListWidget->selectedItems();
	if (items.isEmpty())
		return;

	qSort(items.begin(), items.end(), [&](const QListWidgetItem* i1, const QListWidgetItem* i2) { 
		return ui.fileListWidget->row(i1) < ui.fileListWidget->row(i2);
	});

	std::vector<std::string> sequencesToLoad;
	for (QListWidgetItem* item : items)
		sequencesToLoad.push_back(m_CachedVideoExplorer->getAbsolutePath(item->data(Qt::UserRole).toString().toStdString()));

	//check if selection sequences names are diffrent from the loaded sequences
	std::vector<std::string> loadedSequences = m_SeqManager->getVideosPaths();
	if (sequencesToLoad != loadedSequences)
	{
		blockSignals(true);
		//m_WasPlaying = m_Timeline->isPlaying();
		m_SeqManager->load(sequencesToLoad);
		blockSignals(false);
	}
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::onAutoReloadChecked() {
	EditorConfig::getPointer()->setAutoReload(ui.autoReloadCheckBox->isChecked());
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::tryToReload()
{

	if (m_Timeline->getFrameCount() == 0)
		return;

	if (m_NextFrame >= m_Timeline->getFrameCount()) {
		m_SeqManager->loadNext(0);
	}
	else if (m_NextFrame < 0) {
		m_SeqManager->loadPrev(-1);
	}
}

/*--------------------------------------------------------------------*/
void EditorSequencesWidget::onLoadingFinished()
{
	//if (m_WasPlaying)
	//	m_Timeline->play();

	//m_WasPlaying = false;
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
				//m_WasPlaying = m_Timeline->isPlaying();
				QTimer::singleShot(1, this, SLOT(tryToReload()));
			}
			break;

		case MESSAGE_SEQUENCE_LOADED:
			QTimer::singleShot(1, this, SLOT(reload()));
			onLoadingFinished();
			break;

		case MESSAGE_FRAME_CHANGED:
			ui.fileListWidget->refresh();
			break;

		case MESSAGE_CONFIG_CHANGED:
			ui.fileListWidget->setSelectionMode(EditorConfig::getPointer()->isSequenceMultiSelection() ? QAbstractItemView::ExtendedSelection : QAbstractItemView::SingleSelection);
			break;
	}
}

/*--------------------------------------------------------------------*/
bool EditorSequencesWidget::canAutoReload(int nextFrame)
{
	return ui.autoReloadCheckBox->isChecked() 
		&& ((nextFrame >= m_Timeline->getFrameCount()) || (nextFrame < 0));
}


