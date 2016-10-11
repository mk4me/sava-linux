#pragma once
#ifndef Monitor_h__
#define Monitor_h__

#include <stdlib.h>
namespace boost
{
	namespace serialization
	{
		class access;
	}
}

namespace config
{
	class Monitor
	{
		friend class boost::serialization::access;

	public:
		static Monitor& getInstance();

		bool load();
		bool save() const;

		size_t getAlertsLifetime() const { return m_AlertsLifetime; }
		void setAlertsLifetime(size_t livetime) { m_AlertsLifetime = livetime; }

		size_t getMaxAlertsCount() const { return m_MaxAlertsCount; }
		void setMaxAlertsCount(size_t val) { m_MaxAlertsCount = val; }

	private:
		Monitor();
		~Monitor() { }

		size_t m_AlertsLifetime;	//in hours
		size_t m_MaxAlertsCount;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
}

#endif // Monitor_h__
