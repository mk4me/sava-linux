#include "EditorLayerManager.h"
#include <boost/make_shared.hpp>

/*--------------------------------------------------------------------*/
EditorLayerManager::EditorLayerManager()
{
	m_Layers.push_back(boost::make_shared<EditorLayer>(TRAJECTORY, "Trajectory"));
	m_Layers.push_back(boost::make_shared<EditorLayer>(OBJECTS, "Objects"));
}

/*--------------------------------------------------------------------*/
EditorLayerPtr EditorLayerManager::getLayer(ELayer i_Type) const
{
	for each(EditorLayerPtr layerPtr in m_Layers)
		if (layerPtr->getType() == i_Type)
			return layerPtr;

	return nullptr;
}

/*--------------------------------------------------------------------*/
EditorLayerPtr EditorLayerManager::getLayer(int i_Index) const
{
	if (i_Index >= 0 && i_Index < m_Layers.size())
		return m_Layers[i_Index];

	return nullptr;
}
