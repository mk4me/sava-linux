#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include "QtGui/QColor"
#include <boost/shared_ptr.hpp>
#include <vector>

#include <utils/Singleton.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
//#include <boost/serialization/>

/**
* \class TrajectoryPart
*
* \brief Handle information about trajectory part (its name, marker color, etc.)
*
*/
class TrajectoryPart
{
public:
	TrajectoryPart() {};
	TrajectoryPart(const std::string& name, QColor color = QColor()) : m_Name(name), m_Color(color) {};

	void setName(const std::string& name) { m_Name = name; }
	std::string getName() const { return m_Name; }

	void setColor(const QColor& color) { m_Color = color; }
	QColor getColor() const { return m_Color; }

private:
	friend class boost::serialization::access;

	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
		int r, g, b, a;

		ar >> m_Name;
		ar >> r;
		ar >> g;
		ar >> b;
		ar >> a;

		m_Color = QColor(r, g, b, a);

	};

	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
		int r = m_Color.red();
		int g = m_Color.green();
		int b = m_Color.blue();
		int a = m_Color.alpha();

		ar << m_Name;
		ar << r;
		ar << g;
		ar << b;
		ar << a;
	};

	BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
	std::string m_Name;
	QColor m_Color;
};

typedef boost::shared_ptr<TrajectoryPart> TrajectoryPartPtr;


/**
* \class EditorTrajectoryParts
*
* \brief Conteiner fo EditorTrajectoryPart elements
*
*/
class EditorTrajectoryParts
{

public:
	EditorTrajectoryParts();
	
	bool add(const std::string& name, QColor color = QColor());
	bool remove(const std::string& name);
	bool remove(int index);
	TrajectoryPartPtr get(int index);
	TrajectoryPartPtr get(const std::string& name);
	int count() const { return static_cast<int>(m_Parts.size()); }

	int moveUp(int index);
	int moveDown(int index);

	bool exist(const std::string& name);

	QColor getColorByName(const std::string& name);
	
private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Parts;
	};

	std::vector<TrajectoryPartPtr> m_Parts;
};
