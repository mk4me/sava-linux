#include "EditorLayerElementsList.h"
#include "EditorLayerElementsListDelegate.h"
#include <QtCore/QDebug>
#include <boost/make_shared.hpp>
#include "EditorSingletons.h"
#include "EditorTimeline.h"


EditorLayerElementsList::EditorLayerElementsList(QWidget *parent)
: QListView(parent)
{
	EditorMessageManager::registerMessage(MESSAGE_NEW_SEQUENCE_LOAD_FINISH, this);
}

/*--------------------------------------------------------------------*/
EditorLayerElementsList::~EditorLayerElementsList()
{
	EditorMessageManager::unregisterMessages(this);
}

/*-------------------------------------------------------------------*/
void EditorLayerElementsList::init()
{
	//get list model
	m_ModelView = getListModel();
	assert(m_ModelView);

	//get list filter
	m_FilterView = getListFilter();
	if (m_FilterView)
	{
		m_FilterView->setSourceModel(m_ModelView);
		setModel(m_FilterView);
	}
	else
	{
		setModel(m_ModelView);
	}

	//get list delegate
	m_DelegateView = getListDelegate();
	setItemDelegate(m_DelegateView);

	//get layer
	m_Layer = getLayerType();
}

/*--------------------------------------------------------------------*/
QModelIndex EditorLayerElementsList::insertRow()
{
	QModelIndex addedIndex = mapFromSource(m_ModelView->insertRow());
	if (addedIndex.isValid())
	{
		setCurrentIndex(addedIndex);

		EditorSceneLayerPtr layer = EditorSceneLayerManager::getInstance().getLayer(m_Layer);
		int id = getElementId(addedIndex);
		layer->createElement(id);
		layer->selectElement(id);
	}

	return addedIndex;
}

/*--------------------------------------------------------------------*/
void EditorLayerElementsList::removeRow()
{
	if (currentIndex().isValid())
	{
		EditorSceneLayerPtr layer = EditorSceneLayerManager::getInstance().getLayer(m_Layer);
		layer->deleteElement(getElementId(currentIndex()));
		m_ModelView->removeRow(mapToSource(currentIndex()));
		layer->selectElement(getElementId(currentIndex()));
	}
}

/*--------------------------------------------------------------------*/
void EditorLayerElementsList::selectRow(const QModelIndex& index)
{
	EditorSceneLayerPtr layer = EditorSceneLayerManager::getInstance().getLayer(m_Layer);
	layer->selectElement(getElementId(index));
}

/*--------------------------------------------------------------------*/
void EditorLayerElementsList::unselectRows()
{
	setCurrentIndex(QModelIndex());
	EditorSceneLayerPtr layer = EditorSceneLayerManager::getInstance().getLayer(m_Layer);
	layer->selectElement(-1);
}

/*--------------------------------------------------------------------*/
void EditorLayerElementsList::paintEvent(QPaintEvent *e)
{
	QListView::paintEvent(e);

	if (!viewport()->isEnabled())
		return;

	float progress = EditorTimeline::getInstance().getProgress();
	int lineX = width() * progress;

	QPainter painter(viewport());
	painter.setPen(QColor(255, 255, 255, 100));
	painter.drawLine(lineX, 0, lineX, geometry().height());
}

/*--------------------------------------------------------------------*/
void EditorLayerElementsList::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/)
{
	if (i_Message == MESSAGE_NEW_SEQUENCE_LOAD_FINISH) 
	{
		unselectRows();
		m_FilterView->invalidate();
	}
}

/*--------------------------------------------------------------------*/
void EditorLayerElementsList::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Delete)
		removeRow();

	QListView::keyPressEvent(event);
}

/*--------------------------------------------------------------------*/
QModelIndex EditorLayerElementsList::mapToSource(const QModelIndex& index) const{
	return m_FilterView->mapToSource(index);
}

/*--------------------------------------------------------------------*/
QModelIndex EditorLayerElementsList::mapFromSource(const QModelIndex& index) const{
	return m_FilterView->mapFromSource(index);
}

