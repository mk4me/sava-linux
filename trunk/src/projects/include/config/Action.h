#pragma once
#ifndef Config_Action_h__
#define Config_Action_h__

#include <boost/serialization/access.hpp>

#include <string>
#include <map>

namespace config
{
	class Action
	{
		friend class boost::serialization::access;

	public:
		enum DescriptorType
		{
			GBH = 1,
			MBH = 2,
			GBH_MBH = 3,
		};

		struct Descriptor
		{
			Descriptor(int size, const std::string& name, const std::string& path) : size(size), name(name), path(path)	{ }

			int size;
			std::string name;
			std::string path;
		};

		Action() : m_DescriptorType(GBH_MBH) { }
		~Action() { }

		bool load();
		bool save() const;

		const std::string& getDatabasePath() const { return m_DatabasePath; }
		void setDatabasePath(const std::string& path) { m_DatabasePath = path; }

		DescriptorType getDescriptorType() const { return m_DescriptorType; }
		void setDescriptorType(DescriptorType type) { m_DescriptorType = type; }

		std::string getDescriptorPath() const;
		std::string getDescriptorName() const;
		int getDescriptorSize() const;

		const Descriptor& getDescriptor(DescriptorType type) const;

	private:
		

		static const std::map<DescriptorType, Descriptor> c_Descriptors;

		std::string m_DatabasePath;
		DescriptorType m_DescriptorType;
		
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};

	template<class Archive>
	void Action::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_DatabasePath;
		ar & m_DescriptorType;
	}
}

#endif // Config_Action_h__
