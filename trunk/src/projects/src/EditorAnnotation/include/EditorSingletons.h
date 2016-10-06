
#ifndef EDITOR_SINGLETONS
#define EDITOR_SINGLETONS

#include "EditorSequencesManager.h"
#include "EditorLayerManager.h"
#include "EditorTrajectoryManager.h"
#include "EditorConfig.h"
#include <utils/Glossary.h>

class EditorSingletos
{

private:
	//editor sequence manager
	EditorSequencesManager sequencesManager;

	//editor layer manager
	EditorLayerManager layerManager;

	//editor trajectory manager
	EditorTrajectoryManager trajectoryManager;

	//editor data
	//REMOVE EditorCrumbledStreamData dataManager;

	//configuration (singleton)
	EditorConfig configManager;

	//slownik z adnotacjami
	utils::Glossary glossary;
};

#endif //EDITOR_SINGLETONS

