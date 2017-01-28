#pragma once
#ifndef EDITOR_SINGLETONS
#define EDITOR_SINGLETONS

#include "EditorSequencesManager.h"
#include "EditorLayerManager.h"
#include "EditorConfig.h"
#include <config/Glossary.h>

class EditorSingletons
{
private:
	//editor sequence manager
	EditorSequencesManager sequencesManager;

	//editor layer manager
	EditorLayerManager layerManager;

	//editor data
	//REMOVE EditorCrumbledStreamData dataManager;

	//configuration (singleton)
	EditorConfig configManager;


};

#endif //EDITOR_SINGLETONS

