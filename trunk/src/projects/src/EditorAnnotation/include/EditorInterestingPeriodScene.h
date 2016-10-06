#pragma once
#include "EditorInterestingPeriod.h"
#include "EditorSingletons.h"

/**
* \class EditorInterestingPeriodScene
*
* \brief Class collect all periods time, that include dynamic objects from the current loaded sequence video
*
*/
class EditorInterestingPeriodScene: public EditorInterestingPeriod
{

public:
	EditorInterestingPeriodScene();
	~EditorInterestingPeriodScene();

protected:
	virtual void addPeriods() override;
};