#pragma once
#ifndef Process_h__
#define Process_h__

#include <vector>
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
	class Process
	{
	public:
		typedef std::vector<int64_t> NodeList;
		
		friend class boost::serialization::access;

		static Process& getInstance();

		bool load();
		bool save() const;
	
		int getIdleTimeout() const { return m_IdleTimeout; }
		void setIdleTimeout(int timeout) { m_IdleTimeout = timeout; }

		const NodeList& getAquisitionNodes() const { return m_AquisitionNode; }
		void setAquisitionNodes(const NodeList& val) { m_AquisitionNode = val; }

		const NodeList& getCompressionNodes() const { return m_CompressionNodes; }
		void setCompressionNodes(const NodeList& val) { m_CompressionNodes = val; }

		const NodeList& getPathDetectionNodes() const { return m_PathDetectionNodes; }
		void setPathDetectionNodes(const NodeList& val) { m_PathDetectionNodes = val; }

		const NodeList& getPathAnalysisNodes() const { return m_PathAnalysisNode; }
		void setPathAnalysisNodes(const NodeList& val) { m_PathAnalysisNode = val; }

		const NodeList& getGbhNodes() const { return m_GbhNodes; }
		void setGbhNodes(const NodeList& val) { m_GbhNodes = val; }

		const NodeList& getMbhNodes() const { return m_MbhNodes; }
		void setMbhNodes(const NodeList& val) { m_MbhNodes = val; }

		const NodeList& getSvmNodes() const { return m_SvmNodes; }
		void setSvmNodes(const NodeList& val) { m_SvmNodes = val; }

	private:
		Process() { }
		~Process() { }

		int m_IdleTimeout;

		NodeList m_AquisitionNode;
		NodeList m_CompressionNodes;
		NodeList m_PathDetectionNodes;
		NodeList m_PathAnalysisNode;
		NodeList m_GbhNodes;
		NodeList m_MbhNodes;
		NodeList m_SvmNodes;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
}

#endif // Process_h__
