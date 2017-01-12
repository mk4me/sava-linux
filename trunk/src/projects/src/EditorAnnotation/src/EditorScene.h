#pragma once
#ifndef _EDITOR_SCENE_H_
#define _EDITOR_SCENE_H_

#include "QtWidgets/QGraphicsScene"
#include "QtGui/QPainter"
#include "EditorSceneObject.h"
#include "EditorMessageManager.h"

class EditorSceneLayerManager;


/// <summary>
/// Graficzna scena, wywietlaj¹ca materia³ wideo wraz z wszystkimi warstwami (m.in. na³o¿one obszary klastrów).
/// </summary>
/// <seealso cref="EditorMessageListener" />
class EditorScene : public QGraphicsScene, public EditorMessageListener
{
	Q_OBJECT

public:
	EditorScene(QObject *parent = 0);
	~EditorScene();

	void redraw(int frameNr = -1);

private:
	void reload();

protected:
	virtual void onMessage(EditorMessageType i_Message, int i_ExtraParam = -1) override;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:

	//layer scene manager
	EditorSceneLayerManager* m_LayerManager;


	/**
	* \class SceneBackgroudItem
	*
	* \brief Graphics item to draw scene backgroud
	*
	*/
	class BackgroudGraphicsItem : public QGraphicsPixmapItem 
	{
		public:
			BackgroudGraphicsItem(QGraphicsItem * parent = 0) 
				:QGraphicsPixmapItem(parent)
			{}

			BackgroudGraphicsItem(const QPixmap & pixmap, QGraphicsItem * parent = 0) 
				: QGraphicsPixmapItem(pixmap, parent)
			{}


		protected:
			virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

	}
	*m_BackgroundItem;
};


#endif // _EDITOR_SCENE_H_
