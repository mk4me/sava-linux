#include "EditorSceneLayer.h"
#include <QtCore/QDebug>
#include <QtWidgets/QGraphicsScene>
#include "EditorGraphicsView.h"

/*--------------------------------------------------------------------*/
EditorSceneLayer::EditorSceneLayer(const EditorLayerPtr& i_Layer)
	:m_SourceLayer(i_Layer)
{
	setZValue(m_SourceLayer->getZIndex());
	setFlag(QGraphicsItem::ItemHasNoContents);
	setFlag(QGraphicsItem::ItemIsFocusable);
}

/*--------------------------------------------------------------------*/
EditorGraphicsView* EditorSceneLayer::getView()
{
	static EditorGraphicsView* view = nullptr;
	if (!view)
		view = dynamic_cast<EditorGraphicsView*>(scene()->views().at(0));

	assert(view);
	return view;
}


