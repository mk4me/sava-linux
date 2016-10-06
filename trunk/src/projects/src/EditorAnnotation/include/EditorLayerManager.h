#pragma once

#include <utils/Singleton.h>
#include "EditorLayer.h"
#include <vector>

/**
 * \class EditorLayerManager
 *
 * \brief Stores and manage all editor layers.
 *
 */
class EditorLayerManager: public Singleton<EditorLayerManager>
{

public:
	EditorLayerManager();

	size_t getNumLayers() const { return m_Layers.size(); }

	EditorLayerPtr getLayer(ELayer i_Type) const;
	EditorLayerPtr getLayer(int i_Index) const;

private:
	std::vector<EditorLayerPtr> m_Layers;
};