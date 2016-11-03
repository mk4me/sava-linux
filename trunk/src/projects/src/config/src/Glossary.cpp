#include "Glossary.h"

#include "utils/Filesystem.h"
#include "utils/Database.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>
#include <iostream>

BOOST_CLASS_VERSION(config::Glossary, 1)

namespace config
{
	const std::string Glossary::c_EmptyString;

	template<class Archive>
	void Glossary::ActionStorage::serialize(Archive & ar, const unsigned int version)
	{
		ar & name;
		ar & train;
	}

	template<class Archive>
	void Glossary::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_EditorActions;

		if (version < 1)
		{
			std::vector<int> trainingRemap;
			ar & trainingRemap;
		}

		ar & m_LastId;
	}

	Glossary& Glossary::getInstance()
	{
		static Glossary instance;
		return instance;
	}

	bool Glossary::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "glossary.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Glossary::load() exception: " << e.what() << std::endl;
			return false;
		}

		return m_Database.load();
	}

	bool Glossary::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "glossary.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Glossary::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	Glossary::ActionList Glossary::getEditorActions() const
	{
		ActionList list;
		for (auto it = m_EditorActions.begin(); it != m_EditorActions.end(); ++it)
			list.push_back(Action(it->second.name, it->second.train, it->first));
		
		return list;
	}

	int Glossary::addEditorAction(const std::string& name, bool train)
	{
		if (std::find_if(
			m_EditorActions.begin(),
			m_EditorActions.end(),
			[&](const std::pair<int, ActionStorage>& action) { return action.second.name == name; }
			) != m_EditorActions.end())
			return -1;

		int index = m_LastId++;
		m_EditorActions.insert(std::pair<int, ActionStorage>(index, ActionStorage(name, train)));

		return index;
	}

	bool Glossary::modifyEditorAction(int id, const std::string& name, bool train)
	{
		auto it = std::find_if(
			m_EditorActions.begin(),
			m_EditorActions.end(),
			[&](const std::pair<int, ActionStorage>& action) { return action.second.name == name; }
		);

		if (it != m_EditorActions.end() && it->first != id)
			return false;

		ActionStorage& action = m_EditorActions[id];
		action.name = name;
		action.train = train;

		return true;
	}

	bool Glossary::removeEditorAction(int id)
	{
		return m_EditorActions.erase(id) > 0;
	}

	std::vector<std::string> Glossary::getTrainedActions() const
	{
		std::vector<std::string> actions;
		const auto& ids = m_Database.getActionIdList();
		for (auto id : ids)
			actions.push_back(m_Database.getActionName(id));

		return actions;
	}

	const std::string& Glossary::getEditorActionName(int id) const
	{
		auto it = m_EditorActions.find(id);
		if (it == m_EditorActions.end())
			return c_EmptyString;
		return it->second.name;
	}

	const std::string& Glossary::getTrainedActionName(int id) const
	{
		return m_Database.getActionName(id);
	}

	bool Glossary::isTrained(int id) const
	{
		auto it = m_EditorActions.find(id);
		if (it == m_EditorActions.end())
			return false;
		return it->second.train;
	}
}
