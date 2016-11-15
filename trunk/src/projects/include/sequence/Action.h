#pragma once
#ifndef Action_h__
#define Action_h__

#include <boost/serialization/access.hpp>

#include <string>

namespace sequence
{
	class Action
	{
	public:
		enum Split
		{
			UNASSIGNED = 0,
			TRAIN = 1,
			TEST = 2,
			GENERATED = 3,
		};

		explicit Action(int actionId = -1, Split split = UNASSIGNED) : m_ActionId(actionId), m_Split(split) { }
		explicit Action(const std::string& filename);
		~Action() { }

		bool load(const std::string& filename);
		bool loadFromText(const std::string& filename);
		bool save(const std::string& filename) const;
		bool saveAsText(const std::string& filename) const;

		int getActionId() const { return m_ActionId; }
		void setActionId(int val) { m_ActionId = val; }

		Action::Split getSplit() const { return m_Split; }
		void setSplit(Action::Split val) { m_Split = val; }

	private:
		int m_ActionId;
		Split m_Split;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};

	template<class Archive>
	void Action::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_ActionId;
		ar & m_Split;
	}

}

#endif // Action_h__
