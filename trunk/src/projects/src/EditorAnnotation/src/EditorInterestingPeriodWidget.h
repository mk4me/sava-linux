#pragma once
#ifndef EDITORINTERESTINGPERIODWIDGET_H
#define EDITORINTERESTINGPERIODWIDGET_H

#include <QtWidgets/QGraphicsView>
#include "EditorInterestingPeriod.h"
#include "EditorMessageManager.h"
#include "QtWidgets/QGraphicsLineItem"
#include "EditorSingletons.h"

/**
* \class EditorInterestingPeriodWidget
*
* \brief Widget to show interesting period times (from EditorInterestingPeriod)
*
*/
class EditorInterestingPeriodWidget : public QGraphicsView, public EditorMessageListener
{
	Q_OBJECT

	const static int SCENE_HEIGH = 100;

public:
	EditorInterestingPeriodWidget(QWidget *parent = 0);
	~EditorInterestingPeriodWidget();

public slots:
	/** \brief go to next period */
	void nextPeriod();
	/** \brief go to prev period */
	void prevPeriod();

protected:
	virtual void resizeEvent(QResizeEvent *) override;
	virtual void showEvent(QShowEvent *) override;
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;

private:
	void createLine();
	void createRects();
	void deleteRects();

private:

	class RectItem : public QGraphicsRectItem 
	{
		public:
			RectItem();
		protected:
			virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
			virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
			virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

		private:
			QColor m_NormalColor;
			QColor m_HoverColor;
	};

	QVector<RectItem*> m_RectsItems;
	QGraphicsLineItem* m_LineItem;
	QGraphicsScene* m_Scene;

	EditorInterestingPeriod* m_InterestingPeriods;
};

#endif // EDITORINTERESTINGPERIODWIDGET_H
