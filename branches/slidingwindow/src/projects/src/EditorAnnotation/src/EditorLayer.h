#pragma once

#include <boost/shared_ptr.hpp>

class EditorLayer;
typedef boost::shared_ptr<EditorLayer> EditorLayerPtr;

/*--------------------------------------------------------------------*/
enum ELayer
{
	OBJECTS,
	MAX_LAYER,
};


/// <summary>
/// Reprezentuje pojedyncz¹ warstwê na wywietlanej scenie edytora.
/// </summary>
class EditorLayer
{

public:
	EditorLayer(ELayer i_Type, const std::string& i_Name);

	ELayer getType() const { return m_Type; }
	std::string getName() const { return m_Name; }
	int getZIndex() const { return m_ZIndex; }

private:
	ELayer m_Type;
	std::string m_Name;
	int m_ZIndex;
};
