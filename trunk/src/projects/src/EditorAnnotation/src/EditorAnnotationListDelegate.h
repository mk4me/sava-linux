#pragma once
#ifndef EDITORANNOTATIONLISTDELEGATE_H
#define EDITORANNOTATIONLISTDELEGATE_H


#include <QtWidgets/QStyledItemDelegate>
#include <QtGui/QPainter>

#include "EditorMessageManager.h"
#include "EditorCluster.h"
#include <set>

class EditorAnnotationListDelegate : public QStyledItemDelegate, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorAnnotationListDelegate(QObject *parent);
	~EditorAnnotationListDelegate();

protected:
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

private:
	QRect getClusterSection(const EditorClusterPtr& cluster, const QRect& rect) const;
	void drawClusterKeys(const EditorClusterPtr& cluster, const QRect& rect, QPainter* painter) const;
	void drawClusterContinuation(const EditorClusterPtr& cluster, const QRect& rect, QPainter* painter) const;
	void drawClusterActions(const EditorClusterPtr& cluster, const QRect& rect, QPainter* painter) const;

private:
	std::set<size_t> m_SelectedIds;
	
};

#endif // EDITORANNOTATIONLISTDELEGATE_H
