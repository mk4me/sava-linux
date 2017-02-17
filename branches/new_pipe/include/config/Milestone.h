#pragma once
#ifndef Milestone_h__
#define Milestone_h__

#include <string>

namespace boost
{
	namespace serialization
	{
		class access;
	}
}

namespace config
{
	class Milestone
	{
	public:
		static Milestone& getInstance();

		bool load();
		bool save() const;

		const std::string& getIp() const { return m_Ip; }
		void setIp(const std::string& val) { m_Ip = val; }

		const std::string& getUser() const { return m_User; }
		void setUser(const std::string& val) { m_User = val; }

		const std::string& getPassword() const { return m_Password; }
		void setPassword(const std::string& val) { m_Password = val; }

	private:
		Milestone() { }
		~Milestone() { }

		std::string m_User;
		std::string m_Password;
		std::string m_Ip;

		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
}

#endif // Milestone_h__
