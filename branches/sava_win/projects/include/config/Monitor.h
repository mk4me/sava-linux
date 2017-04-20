#pragma once
#ifndef Monitor_h__
#define Monitor_h__

#include <stdlib.h>
#include <map>
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
		typedef std::map<std::string, std::string> AliasMap;

		static Monitor& getInstance();

		bool load();
		bool save() const;

		size_t getAlertsLifetime() const { return m_AlertsLifetime; }
		void setAlertsLifetime(size_t livetime) { m_AlertsLifetime = livetime; }

		size_t getMaxAlertsCount() const { return m_MaxAlertsCount; }
		void setMaxAlertsCount(size_t count) { m_MaxAlertsCount = count; }

		size_t getMinQueueSize() const { return m_MinQueueSize; }
		void setMinQueueSize(size_t size) { m_MinQueueSize = size; }

		size_t getMaxQueueSize() const { return m_MaxQueueSize; }
		void setMaxQueueSize(size_t size) { m_MaxQueueSize = size; }

		float getTimeScale() const { return m_TimeScale; }
		void setTimeScale(float scale) { m_TimeScale = scale; }

		const std::string& getAlias(const std::string& baseName) const;
		const AliasMap& getAliases() const { return m_Aliases; }
		void setAliases(const AliasMap& aliasMap) { m_Aliases = aliasMap; }

		bool isBackupEnabled() const { return m_IsBackupEnabled; }
		void setBackupEnabled(bool enabled) { m_IsBackupEnabled = enabled; }

		int getDecorationType() const { return m_DecorationType; }
		void setDecorationType(int decorationType) { m_DecorationType = decorationType; }

	private:
		Monitor();
		~Monitor() { }

		size_t m_AlertsLifetime;	//in hours
		size_t m_MaxAlertsCount;
		size_t m_MinQueueSize;
		size_t m_MaxQueueSize;
		float m_TimeScale;
		bool m_IsBackupEnabled;
		int m_DecorationType;

		std::map<std::string, std::string> m_Aliases;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
}

#endif // Monitor_h__
