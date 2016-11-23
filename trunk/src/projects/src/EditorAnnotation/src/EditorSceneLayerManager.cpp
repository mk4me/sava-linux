#include <boost/make_shared.hpp>

#include "EditorSceneLayerManager.h"
#include "EditorSceneTrajectoryLayer.h"
#include "EditorSceneTrajectoryPoint.h"
#include "EditorSceneObjectLayer.h"

/*--------------------------------------------------------------------*/
EditorSceneLayerManager::EditorSceneLayerManager(EditorScene* scene)
:m_Scene(scene)
{
}

/*--------------------------------------------------------------------*/
void EditorSceneLayerManager::init()
{
	EditorLayerManager* layers = EditorLayerManager::getPointer();
	for (int i = 0; i < layers->getNumLayers(); i++)
	{
		EditorSceneLayerPtr sceneLayer = EditorSceneLayerFactory::create(layers->getLayer(i));
		if (sceneLayer)
		{
			m_Layers.push_back(sceneLayer);
			m_Scene->addItem(sceneLayer.get());
			sceneLayer->init();
		}
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneLayerManager::reload()
{
	for (EditorSceneLayerPtr layerPtr : m_Layers) {
		layerPtr->setRect(m_Scene->sceneRect());
		layerPtr->reload();
	}
}

/*--------------------------------------------------------------------*/
void EditorSceneLayerManager::redraw()
{
	for (EditorSceneLayerPtr layerPtr : m_Layers)
		layerPtr->redraw();
}

/*--------------------------------------------------------------------*/
EditorSceneLayerPtr EditorSceneLayerManager::getLayer(ELayer layerType)
{
	auto it = std::find_if(m_Layers.begin(), m_Layers.end(), [layerType](const EditorSceneLayerPtr& layer) { return layer->getSource()->getType() == layerType; });
	if (it != m_Layers.end())
		return *it;
	
	return nullptr;
}


/*--------------------------------------------------------------------*/
void EditorSceneLayerManager::mouseMove(QGraphicsSceneMouseEvent* e)
{
	for (EditorSceneLayerPtr layerPtr : m_Layers)
		layerPtr->mouseMove(e);
}

/*--------------------------------------------------------------------*/
void EditorSceneLayerManager::mouseRelease(QGraphicsSceneMouseEvent*e)
{
	for (EditorSceneLayerPtr layerPtr : m_Layers)
		layerPtr->mouseRelease(e);
}

/*--------------------------------------------------------------------*/
void EditorSceneLayerManager::setLayerEnabled(ELayer layerType)
{
	EditorSceneLayerPtr layerToEnabled = getLayer(layerType);
	for (EditorSceneLayerPtr layerPtr : m_Layers)
	{
		layerPtr->setEnabled(layerPtr == layerToEnabled);
		layerPtr->setZValue((layerPtr == layerToEnabled) ? 1 : 0);
	}	
}


// SCENE LAYER FACTORY
/*--------------------------------------------------------------------*/
EditorSceneLayerPtr EditorSceneLayerFactory::create(const EditorLayerPtr& i_SourceLayer)
{
	if (i_SourceLayer->getType() == TRAJECTORY)
		return boost::make_shared<EditorSceneTrajectoryLayer>(i_SourceLayer);
	if (i_SourceLayer->getType() == OBJECTS)
		return boost::make_shared<EditorSceneObjectLayer>(i_SourceLayer);

	return nullptr;
}



// SCENE LAYER HELPER
/*--------------------------------------------------------------------*/
EditorSceneObjectLayer* EditorSceneLayerHelper::getObjectLayer()
{
	EditorSceneObjectLayer* layer = static_cast<EditorSceneObjectLayer*>(EditorSceneLayerManager::getInstance().getLayer(OBJECTS).get());
	assert(layer);
	return layer;
}

EditorSceneTrajectoryLayer* EditorSceneLayerHelper::getTrajectoryLayer()
{
	EditorSceneTrajectoryLayer* layer = static_cast<EditorSceneTrajectoryLayer*>(EditorSceneLayerManager::getInstance().getLayer(TRAJECTORY).get());
	assert(layer);
	return layer;
}



