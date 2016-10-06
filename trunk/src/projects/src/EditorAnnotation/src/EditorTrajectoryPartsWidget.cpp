#include "EditorTrajectoryPartsWidget.h"
#include "QtWidgets\QComboBox"
#include "QtWidgets\QColorDialog"
#include "boost/random/mersenne_twister.hpp"
#include <boost/random/uniform_int_distribution.hpp>
#include <ctime>

//--------------- TrajectoryPartsWidget ----------------------------------------------------------------------------------------------------------------------------------------------
EditorTrajectoryPartsWidget::EditorTrajectoryPartsWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);

	m_Model = new TrajectoryPartsTableViewModel(tableView);
	tableView->setModel(m_Model);
	tableView->setItemDelegate(new TrajectoryPartsTableDelegate(tableView));

	pointRadiusSpinBox->setValue(EditorConfig::getInstance().getTrajectoryPointRadius());
	lineWidthSpinBox->setValue(EditorConfig::getInstance().getTrajectoryLineWidth());

}

/*--------------------------------------------------------------------*/
void EditorTrajectoryPartsWidget::onItemPressed(const QModelIndex& index)
{
	if (!index.isValid())
		return;

	if (index.column() == 1)	//color 
	{
		QModelIndex nameIndex = index.model()->index(index.row(), 0, index.parent());

		QColorDialog dialog;
		dialog.setCurrentColor(index.data().value<QColor>());
		dialog.setWindowTitle(nameIndex.data().toString());
		dialog.exec();
		
		TrajectoryPartPtr part = EditorConfig::getInstance().getTrajectoryParts().get(index.row());
		if (part)
			part->setColor(dialog.currentColor());

		tableView->update(index);

		emit dataChanged();
	}
}

/*--------------------------------------------------------------------*/
void EditorTrajectoryPartsWidget::onAdd()
{
	if (newNameLineEdit->text().isEmpty())
		return;

	//generate random color
	boost::random::mt19937 rng;
	boost::random::uniform_int_distribution<> randomColor(0, 255);
	rng.seed(static_cast<unsigned int>(std::time(0)));

	int r = randomColor(rng);
	int g = randomColor(rng);
	int b = randomColor(rng);

	m_Model->add(newNameLineEdit->text().toStdString(), QColor(r, g, b));
	newNameLineEdit->clear();

	emit dataChanged();
}

/*--------------------------------------------------------------------*/
void EditorTrajectoryPartsWidget::onRemove()
{
	if (tableView->currentIndex().isValid())
	{
		m_Model->remove(tableView->currentIndex().row());
		emit dataChanged();
	}

}

/*--------------------------------------------------------------------*/
void EditorTrajectoryPartsWidget::onMoveUp()
{
	if (tableView->currentIndex().isValid())
	{
		int newIndex = m_Model->moveUp(tableView->currentIndex().row());
		tableView->setCurrentIndex(m_Model->index(newIndex, 0));
		emit dataChanged();
	}
}

/*--------------------------------------------------------------------*/
void EditorTrajectoryPartsWidget::onMoveDown()
{
	if (tableView->currentIndex().isValid())
	{
		int newIndex = m_Model->moveDown(tableView->currentIndex().row());
		tableView->setCurrentIndex(m_Model->index(newIndex, 0));
		emit dataChanged();
	}
}

/*-------------------------------------------------------------------*/
void EditorTrajectoryPartsWidget::onRadiusPointChanged(int value)
{
	EditorConfig::getInstance().setTrajectoryPointRadius(value);
}

/*-------------------------------------------------------------------*/
void EditorTrajectoryPartsWidget::onLineWidthChanged(int value)
{
	EditorConfig::getInstance().setTrajectoryLineWidth(value);
}


//-------------- TrajectoryPartsTableViewModel---------------------------------------------------------------------------------------------------------------------------------------
/*--------------------------------------------------------------------*/
int TrajectoryPartsTableViewModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const{
	return EditorConfig::getPointer()->getTrajectoryParts().count();
}

/*--------------------------------------------------------------------*/
int TrajectoryPartsTableViewModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const{
	return 2;
}

/*--------------------------------------------------------------------*/
QVariant TrajectoryPartsTableViewModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::DisplayRole && index.isValid())
	{
		TrajectoryPartPtr part = EditorConfig::getPointer()->getTrajectoryParts().get(index.row());
		if (part)
		{
			if (index.column() == 0)
				return QString::fromStdString(part->getName());

			if (index.column() == 1)
				return part->getColor();
		}
	}

	return QVariant();
}

/*--------------------------------------------------------------------*/
QVariant TrajectoryPartsTableViewModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (section)
		{
			case 0: return "Name";
			case 1: return "Color";
			case 2: return "Marker";
		}
	}

	return QAbstractTableModel::headerData(section, orientation, role);
}

/*--------------------------------------------------------------------*/
Qt::ItemFlags TrajectoryPartsTableViewModel::flags(const QModelIndex &index) const{
	return QAbstractTableModel::flags(index);
}

/*--------------------------------------------------------------------*/
void TrajectoryPartsTableViewModel::add(const std::string& name, QColor color)
{
	beginResetModel();
	EditorConfig::getPointer()->getTrajectoryParts().add(name, color);
	endResetModel();
}

/*--------------------------------------------------------------------*/
void TrajectoryPartsTableViewModel::remove(int index)
{
	beginResetModel();
	EditorConfig::getPointer()->getTrajectoryParts().remove(index);
	endResetModel();
}

/*--------------------------------------------------------------------*/
int TrajectoryPartsTableViewModel::moveUp(int index)
{
	beginResetModel();
	int ret = EditorConfig::getPointer()->getTrajectoryParts().moveUp(index);
	endResetModel();

	return ret;
}

/*--------------------------------------------------------------------*/
int TrajectoryPartsTableViewModel::moveDown(int index)
{
	beginResetModel();
	int ret = EditorConfig::getPointer()->getTrajectoryParts().moveDown(index);
	endResetModel();

	return ret;
}


//-------------- TrajectoryPartsTableDelegate--------------------------------------------------------------------------------------------------------------------------
/*--------------------------------------------------------------------*/
TrajectoryPartsTableDelegate::TrajectoryPartsTableDelegate(QObject* parent) : QStyledItemDelegate(parent){
	m_TableViewParent = qobject_cast<QTableView*>(parent);
}

/*--------------------------------------------------------------------*/
void TrajectoryPartsTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.isValid())
	{
		painter->save();

		//draw selected item
		if (m_TableViewParent->currentIndex().isValid() && m_TableViewParent->currentIndex().row() == index.row())
			painter->fillRect(option.rect, Qt::darkGray);

		if (index.column() == 0)
		{
			QFont font;
			font.setBold(true);
			painter->setFont(font);
			painter->setPen(Qt::white);
			painter->drawText(option.rect, index.data().toString(), QTextOption(Qt::AlignCenter));
		}
		else if (index.column() == 1)
		{
			painter->fillRect(option.rect, index.data().value<QColor>());
		}


		painter->restore();

	}
}
