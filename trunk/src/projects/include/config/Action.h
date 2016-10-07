#pragma once
#ifndef Config_Action_h__
#define Config_Action_h__

#include <boost/serialization/access.hpp>

#include <string>
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

		static Action& getInstance();

		bool load();
		bool save() const;

		int getMinVideoFrames() const { return m_MinVideoFrames; }
		void setMinVideoFrames(int minFrames) { m_MinVideoFrames = minFrames; }

		unsigned getMaxVideoWidth() const { return m_MaxVideoWidth; }
		void setMaxVideoWidth(unsigned width) { m_MaxVideoWidth = width; }

		unsigned getMaxVideoHeight() const { return m_MaxVideoHeight; }
		void setMaxVideoHeight(unsigned height) { m_MaxVideoHeight = height; }

		int getTrainSetLength() const { return m_TrainSetLength; }
		void setTrainSetLength(int length) { m_TrainSetLength = length; }

		int getTestSetLength() const { return m_TestSetLength; }
		void setTestSetLength(int length) { m_TestSetLength = length; }

		DescriptorType getDescriptorType() const { return m_DescriptorType; }
		void setDescriptorType(DescriptorType type) { m_DescriptorType = type; }

		std::string getDescriptorPath() const;
		std::string getDescriptorName() const;
		int getDescriptorSize() const;

		const Descriptor& getDescriptor(DescriptorType type) const;

	private:
		Action();
		~Action() { }

		static const std::map<DescriptorType, Descriptor> c_Descriptors;

		DescriptorType m_DescriptorType;
		int m_MinVideoFrames;
		int m_TrainSetLength;
		int m_TestSetLength;
		unsigned m_MaxVideoWidth;
		unsigned m_MaxVideoHeight;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
}

#endif // Config_Action_h__
