#pragma once

#include <string>
#include <vector>

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

namespace sequence
{
	class ComplexAction
	{
		public:
			static const int INVALID_COMPLEX_ACTION = -1;

			explicit ComplexAction(const std::string& filename);
			explicit ComplexAction(int complexActionId = INVALID_COMPLEX_ACTION) : m_ComplexActionId(complexActionId) { }
			~ComplexAction() { }

			bool load(const std::string& filename);
			bool loadFromText(const std::string& filename);
			bool save(const std::string& filename) const;
			bool saveAsText(const std::string& filename) const;

			int getComplexActionId() const { return m_ComplexActionId; }
			void setComplexActionId(int id) { m_ComplexActionId = id; }

			std::vector<std::string> getClusters() { return m_Clusters; }
			void setClusters(const std::vector<std::string>& clusters) { m_Clusters = clusters; }

			std::string toString() const;

		private:
			int m_ComplexActionId;
			std::vector<std::string> m_Clusters;

			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
	};


	template<class Archive>
	void ComplexAction::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_ComplexActionId;
		ar & m_Clusters;
	}
}