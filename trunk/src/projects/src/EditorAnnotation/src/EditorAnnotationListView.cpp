#include "EditorAnnotationListView.h"
#include "EditorTimeline.h"
#include "EditorSceneLayerManager.h"

#include <QtWidgets/QHeaderView>
#include <QtGui/QPainter>
#include <QtWidgets/QScrollBar>

/*--------------------------------------------------------------------*/
EditorAnnotationListView::EditorAnnotationListView(QWidget *parent)
	: QListView(parent)
{
	m_Model = new EditorAnnotationListModel(this);
	setModel(m_Model);
	setItemDelegate(new EditorAnnotationListDelegate(this));

	EditorMessageManager::registerMessage(MESSAGE_NEW_SEQUENCE_LOAD_START, this);
	EditorMessageManager::registerMessage(MESSAGE_NEW_SEQUENCE_LOAD_FINISH, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_SELECTION_CHANGED, this);
	EditorMessageManager::registerMessage(MESSAGE_ADD_ACTION, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_OBJECT_CREATED, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_OBJECT_DELETED, this);
	EditorMessageManager::registerMessage(MESSAGE_SCENE_UPDATE, this);
}

/*--------------------------------------------------------------------*/
EditorAnnotationListView::~EditorAnnotationListView(){
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListView::paintEvent(QPaintEvent *e)
{
	QListView::paintEvent(e);

	if (!viewport()->isEnabled())
		return;

	float progress = EditorTimeline::getInstance().getProgress();
	int lineX = (width() - verticalScrollBar()->width()) * progress;

	QPainter painter(viewport());
	painter.setPen(QColor(255, 255, 255, 100));
	painter.drawLine(lineX, 0, lineX, geometry().height());	
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListView::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Delete)
		EditorSceneLayerHelper::getObjectLayer()->deleteSelectedObject();

	QListView::keyReleaseEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListView::scrollToSelected()
{
	int selectedId = EditorSceneLayerHelper::getObjectLayer()->getSelectedObjectId();
	if (selectedId < 0)
		return;

	QModelIndex mIndex  = m_Model->getIndexByInternalId(selectedId);
	if (mIndex.isValid())
		scrollTo(mIndex);
}

/*--------------------------------------------------------------------*/
void EditorAnnotationListView::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	switch (i_Message)
	{
	case MESSAGE_NEW_SEQUENCE_LOAD_START:
		m_Model->beginReset();
		break;

	case MESSAGE_NEW_SEQUENCE_LOAD_FINISH:
		m_Model->update();
		m_Model->endReset();
		break;

	case MESSAGE_SCENE_OBJECT_DELETED:
	case MESSAGE_SCENE_OBJECT_CREATED:
		m_Model->beginReset();
		m_Model->update();
		m_Model->endReset();
		scrollToSelected();

	case MESSAGE_SCENE_UPDATE:
	case MESSAGE_ADD_ACTION:
		m_Model->refresh();
		break;

	case MESSAGE_SCENE_SELECTION_CHANGED:
		m_Model->refresh();
		scrollToSelected();
		break;
	}
}

