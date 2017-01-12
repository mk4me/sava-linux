#pragma once
#ifndef _EDITOR_SCENE_OBJECT_H_
#define _EDITOR_SCENE_OBJECT_H_

#include <QtWidgets/QGraphicsRectItem>
#include <QtGui/QPainter>
#include <QtGui/QColor>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include "EditorCluster.h"

class EditorSceneContextMenu;


/// <summary>
/// Klasa reprezentuje obiekt na scenie. W wypadku edytora jest to reprezentacja graficzna klastra.
/// </summary>
class EditorSceneObject : public QObject, public QGraphicsRectItem
{
	Q_OBJECT

	static int PEN_WIDTH;
	static QColor SELECT_COLOR;
	static QColor UNSELECT_COLOR;

public:
	EditorSceneObject(size_t i_Id, QGraphicsItem *parent = 0);
	~EditorSceneObject(){};

	void setId(size_t i_Id) { m_Id = i_Id; }
	size_t  getId() const { return m_Id;  }

	void setSelected(bool i_Selected) { m_IsSelected = i_Selected; }
	bool isSelected() const { return m_IsSelected;  }

	void updateRegion();

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	virtual QRectF boundingRect() const override;

private:
	void drawLabel(QPainter* painter);


	size_t m_Id;
	bool m_IsSelected;
	EditorClusterPtr m_Cluster;
};

#endif // _EDITOR_SCENE_OBJECT_H_
