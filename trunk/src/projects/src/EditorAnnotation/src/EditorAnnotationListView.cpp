#include "EditorAnnotationListView.h"
#include "EditorTimeline.h"
#include "EditorSceneLayerManager.h"

#include <QtWidgets/QHeaderView>
#include <QtGui/QPainter>
#include <QtWidgets/QScrollBar>

/*--------------------------------------------------------------------*/
EditorAnnotationListView::EditorAnnotationListView(QWidget *parent)
	: QTableView(parent)
{
	m_Model = new EditorAnnotationListModel(this);
	setModel(m_Model);
	setItemDelegate(new EditorAnnotationListDelegate(this));

	EditorMessageManager::registerMessage(MESSAGE_SEQUENCE_LOADED, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_SELECTION_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_ACTION_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_OBJECT_CREATED, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_OBJECT_DELETED, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_OBJECT_UPDATE, this);
}

/*--------------------------------------------------------------------*/
EditorAnnotationListView::~EditorAnnotationListView(){
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListView::paintEvent(QPaintEvent *e)
{
	QTableView::paintEvent(e);

	if (!viewport()->isEnabled())
		return;

	float progress = EditorTimeline::getInstance().getProgress();
	int lineX = columnViewportPosition(0) + columnWidth(0) * progress;

	QPainter painter(viewport());
	painter.setPen(QColor(255, 255, 255, 100));
	painter.drawLine(lineX, 0, lineX, geometry().height());	
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListView::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Delete)
		EditorSceneLayerHelper::getObjectLayer()->deleteSelectedObject();

	QTableView::keyReleaseEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListView::scrollToSelected()
{
	std::set<size_t> selectedIds = EditorSceneLayerHelper::getObjectLayer()->getSelectedObjectsIds();
	if (selectedIds.empty())
		return;

	QModelIndex mIndex = m_Model->getIndexByInternalId(*selectedIds.begin());
	if (mIndex.isValid())
		scrollTo(mIndex);
}



/*--------------------------------------------------------------------*/
void EditorAnnotationListView::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	switch (i_Message)
	{

	case MESSAGE_SEQUENCE_LOADED:
		m_Model->beginReset();
		m_Model->update();
		m_Model->endReset();
		update();
		break;

	case MESSAGE_SCENE_OBJECT_DELETED:
	case MESSAGE_SCENE_OBJECT_CREATED:
		m_Model->beginReset();
		m_Model->update();
		m_Model->endReset();
		scrollToSelected();
		update();
		break;

	case MESSAGE_SCENE_OBJECT_UPDATE:
	case MESSAGE_ACTION_CHANGED:
		m_Model->beginReset();
		m_Model->update();
		m_Model->endReset();
		update();
		break;

	case MESSAGE_SCENE_SELECTION_CHANGED:
		m_Model->refresh();
		scrollToSelected();
		update();
		break;
	}
}

