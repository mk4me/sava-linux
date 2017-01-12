#pragma once

#include <boost/shared_ptr.hpp>
#include "QtWidgets/QGraphicsItem"
#include "EditorLayer.h"
#include "EditorScene.h"

class EditorGraphicsView;

/// <summary>
/// Graficzna reprezentacja EditorLayer.
/// <seealso cref="EditorLayer" />
/// </summary>
class EditorSceneLayer : public QObject, public QGraphicsItem
{
	Q_OBJECT
    //Q_INTERFACES(QGraphicsItem)

public:
	EditorSceneLayer(const EditorLayerPtr& i_Layer);
	virtual ~EditorSceneLayer() { }

	EditorLayerPtr getSource() const { return m_SourceLayer; }

	virtual void init() {};
	virtual void reload() {};
	virtual void redraw() {};

	//functionality to manage layer elements
	virtual void createElement(int id) { };
	virtual void selectElement(int id) { };
	virtual void deleteElement(int id) { };

	/**
	* Set graphics layer rectangle
	*/
	void setRect(QRectF i_Rect) { m_Rect = i_Rect; }

protected:
	EditorGraphicsView* getView();

	QRectF boundingRect() const override { return m_Rect; }
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override { };

	virtual void mouseMove(QGraphicsSceneMouseEvent* e) {};
	virtual void mouseRelease(QGraphicsSceneMouseEvent *e) {};

private:
	EditorLayerPtr m_SourceLayer;
	QRectF m_Rect;

	friend EditorSceneLayerManager;
};

typedef boost::shared_ptr<EditorSceneLayer> EditorSceneLayerPtr;