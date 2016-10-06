#ifndef EDITORANNOTATIONLISTDELEGATE_H
#define EDITORANNOTATIONLISTDELEGATE_H


#include <QtWidgets\QStyledItemDelegate>
#include <QtGui\QPainter>

class EditorAnnotationListDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	EditorAnnotationListDelegate(QObject *parent);
	~EditorAnnotationListDelegate();

protected:
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	QRect getClusterSection(size_t clusterId, const QRect& i_PaintRect) const;
	void drawActionText(size_t _clusterId, const QRect& _paintRect, QPainter* _painter) const;
	
};

#endif // EDITORANNOTATIONLISTDELEGATE_H
