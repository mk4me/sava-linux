#pragma once
#ifndef __GLOSSARY_H__
#define __GLOSSARY_H__

#include "utils\Database.h"

#include <map>
#include <vector>

namespace boost
{
	namespace serialization
	{
		class access;
	}
}

namespace config
{
	class Glossary
	{
		friend class boost::serialization::access;

	public:
		struct Action
		{
			Action(const std::string& name, bool train, int id) : name(name), train(train), id(id) { }

			std::string name;
			bool train;
			int id;
		};

		typedef std::vector<Action> ActionList;
		typedef std::vector<int> TrainingRemap;

		static Glossary& getInstance();

		bool load();
		bool save() const;

		ActionList getEditorActions() const;
		/// <summary>
		/// Adds the editor action.
		/// </summary>
		/// <param name="name">Action unique name.</param>
		/// <param name="train">If set to true action is included in training.</param>
		/// <returns>Action id if successful otherwise -1</returns>
		int addEditorAction(const std::string& name, bool train);
		bool modifyEditorAction(int id, const std::string& name, bool train);
		bool removeEditorAction(int id);

		std::vector<std::string> getTrainedActions() const;

		const std::string& getEditorActionName(int id) const;
		const std::string& getTrainedActionName(int id) const;

		/// <summary>
		/// Determines whether the specified identifier is included in training.
		/// </summary>
		/// <param name="id">The action id.</param>
		/// <returns>True if action is included in training otherwise false</returns>
		bool isTrained(int id) const;

	private:
		class ActionStorage
		{
			friend class boost::serialization::access;

		public:
			ActionStorage(const std::string& name, bool train) : name(name), train(train) { }
			ActionStorage() { }

			std::string name;
			bool train;

		private:
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

		Glossary() : m_LastId(0) { }
		~Glossary() { }

		static const std::string c_EmptyString;

		std::map<int, ActionStorage> m_EditorActions;
		int m_LastId;

		utils::Database m_Database;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
}

#endif // __GLOSSARY_H__