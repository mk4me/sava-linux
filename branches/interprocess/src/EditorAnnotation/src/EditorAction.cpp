#include "EditorAction.h"

/************************************** ACTION **************************************/
EditorAction::EditorAction(const ActionPtr& action)
	: m_Source(action)
	, m_StartFrame(0)
	, m_EndFrame(0)
{

}

EditorAction::EditorAction()
	: EditorAction(std::make_shared<Action>()) 
{ 
}

EditorAction::EditorAction(const std::string& filepath)
	: EditorAction(std::make_shared<Action>(filepath)) 
{ 
}

EditorAction::EditorAction(const EditorAction& action)
{
	m_Source = std::make_shared<Action>();
	m_Source->setActionId(action.getSource()->getActionId());
	m_Source->setSplit(action.getSource()->getSplit());

	m_StartFrame = action.getStartFrame();
	m_EndFrame = action.getEndFrame();
}

void EditorAction::clear()
{
	if (m_Source)
	{
		m_Source->setActionId(-1);
		m_Source->setSplit(sequence::Action::Split::UNASSIGNED);
	}
}


