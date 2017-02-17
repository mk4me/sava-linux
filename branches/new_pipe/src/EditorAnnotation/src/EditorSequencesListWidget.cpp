#include "EditorSequencesListWidget.h"
#include "QtWidgets/QApplication"
#include "EditorSequencesManager.h"
#include "EditorTimeline.h"

EditorSequencesListWidget::EditorSequencesListWidget(QWidget *parent)
	: QListWidget(parent)
{
	m_Delegate = new ListDelegate(this);
	setItemDelegate(m_Delegate);
}

void EditorSequencesListWidget::refresh()
{
	auto items = selectedItems();
	if (items.isEmpty())
		return;

	int firstNr = row(items.at(0));
	int offset = EditorSequencesManager::getInstance().getSequence()->getSequenceNr(EditorTimeline::getInstance().getCurrentFrame());
	int realRowNr = firstNr + offset;

	if (realRowNr != m_Delegate->m_SeqIndex)
	{
		m_Delegate->m_SeqIndex = realRowNr;
		update();
	}

}

void EditorSequencesListWidget::mouseReleaseEvent(QMouseEvent *e)
{
	if (canSelect())
	{
		QListWidget::mouseReleaseEvent(e);
		emit mouseReleased();
	}
}

void EditorSequencesListWidget::mousePressEvent(QMouseEvent *e)
{
	if (canSelect())
	{
		m_Delegate->m_SeqIndex = -1;
		refresh();
		QListWidget::mousePressEvent(e);
	}
}

void EditorSequencesListWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (canSelect())
		QListWidget::mouseDoubleClickEvent(e);
}

void EditorSequencesListWidget::mouseMoveEvent(QMouseEvent *e)
{
	if (canSelect())
		QListWidget::mouseMoveEvent(e);
}

void EditorSequencesListWidget::keyPressEvent(QKeyEvent *e){
	QWidget::keyPressEvent(e);
}

void EditorSequencesListWidget::keyReleaseEvent(QKeyEvent *e) {
	QWidget::keyReleaseEvent(e);
}

bool EditorSequencesListWidget::canSelect() const
{
	return !QApplication::keyboardModifiers().testFlag(Qt::ControlModifier);
}


void EditorSequencesListWidget::ListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	if (index.row() == m_SeqIndex)
	{
		painter->save();

		painter->setRenderHint(QPainter::Antialiasing);

		painter->setPen(QPen(QColor(0, 0, 0), 1));
		painter->setBrush(QColor(255, 139, 30));

		QPolygonF polygon;
		polygon << option.rect.topLeft();
		polygon << QPointF(15, option.rect.y() + option.rect.height() / 2);
		polygon << option.rect.bottomLeft();
		polygon << option.rect.topLeft();
		painter->drawPolygon(polygon);

		//painter->setOpacity(0.6);
		//painter->setBrush(QBrush());
		//painter->drawRoundedRect(option.rect.adjusted(0, 1, 0, -1), 5, 5);

		painter->restore();
	}
}
