#include "EditorLayer.h"

/*--------------------------------------------------------------------*/
EditorLayer::EditorLayer(ELayer i_Type, const std::string& i_Name)
:m_Type(i_Type),
m_Name(i_Name)
{
	m_ZIndex = i_Type + 1;
}
