#pragma once

#include "EditorSceneLayer.h"
#include "EditorScene.h"
#include <vector>
#include "EditorSceneObjectLayer.h"


/// <summary>
/// Przechowuje i zarz¹dza warstwami graficznymi (EditorSceneLayer) na scenie.
/// </summary>
/// <seealso cref="Singleton{EditorSceneLayerManager}" />
class EditorSceneLayerManager : public QObject, public Singleton < EditorSceneLayerManager >
{
	Q_OBJECT

public:
	EditorSceneLayerManager(EditorScene* scene);

	void init(); 
	void reload();
	void redraw();

	/**
	 * set enabled only given enum layer, disable rest of them
	 * \param layerType type of layer (from ELayer)
	 */
	void setLayerEnabled(ELayer layerType);
	EditorSceneLayerPtr getLayer(ELayer layerType);

	void mouseMove(QGraphicsSceneMouseEvent*);
	void mouseRelease(QGraphicsSceneMouseEvent*);

private:
	EditorScene* m_Scene;
	std::vector<EditorSceneLayerPtr> m_Layers;
};

/**
 * \class EditorSceneLayerFactory
 *
 * \brief Factory to create scene layer, depends od EditorLayer
 *
 */
class EditorSceneLayerFactory
{
public:
	static EditorSceneLayerPtr create(const EditorLayerPtr& i_SourceLayer);
};

/**
* \class EditorSceneLayerHelper
*
*	\brief Help functions to manage scene layers
*
*/
class EditorSceneLayerHelper
{
public:
	static EditorSceneObjectLayer* getObjectLayer();
};

