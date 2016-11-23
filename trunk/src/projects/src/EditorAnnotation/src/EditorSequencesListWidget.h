#ifndef EDITORSEQUENCESLISTWIDGET_H
#define EDITORSEQUENCESLISTWIDGET_H

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QStyledItemDelegate>

class EditorSequencesListWidget : public QListWidget
{
	Q_OBJECT

signals:
	void mouseReleased();

public:
	EditorSequencesListWidget(QWidget *parent = 0);
	~EditorSequencesListWidget(){};

	void refresh();

protected:
	virtual void mouseReleaseEvent(QMouseEvent *) override;
	virtual void mousePressEvent(QMouseEvent *) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *) override;
	virtual void mouseMoveEvent(QMouseEvent *) override;

	virtual void keyPressEvent(QKeyEvent *) override;
	virtual void keyReleaseEvent(QKeyEvent *) override;

private:
	bool canSelect() const;


	class ListDelegate : public QStyledItemDelegate
	{
		public:
			ListDelegate(QObject * parent = 0) : QStyledItemDelegate(parent) { }
			virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
			int  m_SeqIndex;
	};

	ListDelegate* m_Delegate;
	
};

#endif // EDITORSEQUENCESLISTWIDGET_H
