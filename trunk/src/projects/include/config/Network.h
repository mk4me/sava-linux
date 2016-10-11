#pragma once
#ifndef Network_h__
#define Network_h__

#include <vector>
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
	class Network
	{
		friend class boost::serialization::access;

	public:
		static Network& getInstance();

		bool load();
		bool save() const;

		int getNumNodes() const { return (int)m_Nodes.size(); }
		
		int addNode(const std::string& name, const std::string& ip);
		bool removeNode(int num);

		const std::string& getName(int num) const;
		bool setName(int num, const std::string& name);

		const std::string& getIp(int num) const;
		bool setIp(int num, const std::string& ip);

		int64_t getId(int num) const;

		int getTcpPort() const { return m_TcpPort; }
		void setTcpPort(int port) { m_TcpPort = port; }

		const std::string& findName(int64_t id) const;
		const std::string& findIp(int64_t id) const;

	private:
		Network() : m_LastId(0), m_TcpPort(1) { }
		~Network() { }

		struct Node
		{
			friend class boost::serialization::access;

			Node() : m_Id(-1) { }
			Node(const std::string& name, const::std::string& ip, int64_t id) : m_Name(name), m_Ip(ip), m_Id(id) { }

			std::string m_Name;
			std::string m_Ip;
			int64_t m_Id;

			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

		static const std::string c_EmptyString;
		static const std::string c_NotFoundString;

		std::vector<Node> m_Nodes;
		int64_t m_LastId;
		int m_TcpPort;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
	};
}

#endif // Network_h__
