#ifndef EDITORTRAJECTORYPARTSWIDGET_H
#define EDITORTRAJECTORYPARTSWIDGET_H

#include "ui_EditorTrajectoryPartsWidget.h"
#include <QtWidgets/QWidget>
#include "QtCore/QAbstractItemModel"
#include "QtWidgets/QStyledItemDelegate"
#include <QtGui/QPainter>
#include "QtWidgets/QTableView"
#include "EditorSingletons.h"

class TrajectoryPartsTableViewModel;

/**
 * \class EditorTrajectoryPartsWidget
 *
 * \brief widget to vizualize trajectory parts
 *
 * \author dev
 * \date maj 2015
 */
class EditorTrajectoryPartsWidget : public QWidget, public Ui::EditorTrajectoryPartsWidget
{
	Q_OBJECT

public:
	EditorTrajectoryPartsWidget(QWidget *parent = 0);

signals:
	void dataChanged();

private slots:
	void onItemPressed(const QModelIndex& index);
	void onAdd();
	void onRemove();
	void onMoveUp();
	void onMoveDown();
	void onRadiusPointChanged(int value);
	void onLineWidthChanged(int value);

private:
	TrajectoryPartsTableViewModel* m_Model;
};


/**
 * \class TrajectoryPartsTableViewModel
 *
 * \brief view model for trajecotry parts table
 *
 * \author dev
 * \date maj 2015
 */
class TrajectoryPartsTableViewModel : public QAbstractTableModel
{
public:
	TrajectoryPartsTableViewModel(QObject* parent = 0) : QAbstractTableModel(parent) {}

	void add(const std::string& name, QColor color);
	void remove(int index);
	int moveUp(int index);
	int moveDown(int index);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

};

/**
 * \class TrajectoryPartsTableDelegate
 *
 * \brief delegate for trajectory parts table 
 *
 * \author dev
 * \date maj 2015
 */
class TrajectoryPartsTableDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	TrajectoryPartsTableDelegate(QObject* parent);

protected:
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	QTableView* m_TableViewParent;

};

#endif // EDITORTRAJECTORYPARTSWIDGET_H
