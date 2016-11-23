#include "EditorGlossaryWidget.h"
#include "QtCore/QDebug"

EditorGlossaryWidget::EditorGlossaryWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//init tree view
	m_ModelView = new EditorGlossaryModel(this);
	m_Filter.setSourceModel(m_ModelView);
	m_Filter.setDynamicSortFilter(true);

	ui.treeView->setModel(&m_Filter);
	connect(ui.treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
	connect(ui.treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onClicked(QModelIndex)));

	EditorMessageManager::registerMessage(MESSAGE_SEQUENCE_LOADED, this);
}

/*--------------------------------------------------------------------*/
EditorGlossaryWidget::~EditorGlossaryWidget() {
	EditorMessageManager::unregisterMessages(this);
}

/*--------------------------------------------------------------------*/
void EditorGlossaryWidget::onMessage(EditorMessageType i_Message, int i_ExtraParam /*= -1*/) {
	m_ModelView->reload();
}

/*--------------------------------------------------------------------*/
void EditorGlossaryWidget::filterChanged(const QString& text)
{
	if (!text.isEmpty())
		ui.treeView->expandAll();
	else
		ui.treeView->collapseAll();

	m_Filter.setFilterRegExp(QRegExp(text, Qt::CaseInsensitive));
}

/*--------------------------------------------------------------------*/
void EditorGlossaryWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->modifiers() == Qt::CTRL)
		ui.treeView->setSelectionMode(QAbstractItemView::MultiSelection);

	QWidget::keyPressEvent(event);
}

/*--------------------------------------------------------------------*/
void EditorGlossaryWidget::onClicked(const QModelIndex& index)
{
	if (!QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) && ui.treeView->selectionMode() == QAbstractItemView::MultiSelection )	{
		ui.treeView->setSelectionMode(QAbstractItemView::SingleSelection);	
		ui.treeView->setCurrentIndex(index);
	} else {
		ui.treeView->setSelectionMode(QAbstractItemView::MultiSelection);
	}

}

/*--------------------------------------------------------------------*/
void EditorGlossaryWidget::onDoubleClicked(const QModelIndex& index)
{
	ui.treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.treeView->setCurrentIndex(index);

	emit doubleClicked(index);
}
