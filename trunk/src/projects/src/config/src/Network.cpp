#include "Network.h"

#include "utils/Filesystem.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/access.hpp>

#include <fstream>

namespace config
{
	const std::string Network::c_EmptyString;
	const std::string Network::c_NotFoundString = "<Not found>";

	template<class Archive>
	void config::Network::Node::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Name;
		ar & m_Ip;
		ar & m_Id;
	}

	template<class Archive>
	void config::Network::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_LastId;
		ar & m_Nodes;
		ar & m_TcpPort;
	}

	Network& Network::getInstance()
	{
		static Network instance;
		return instance;
	}

	bool Network::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "network.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Network::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Network::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "network.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Network::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	int Network::addNode(const std::string& name, const std::string& ip)
	{
		if (name.empty() || ip.empty())
			return -1;

		if (std::find_if(m_Nodes.begin(), m_Nodes.end(), [&](const Node& n) { return name == n.m_Name || ip == n.m_Ip; }) != m_Nodes.end())
			return -1;

		m_Nodes.push_back(Node(name, ip, m_LastId++));
		return (int)m_Nodes.size() - 1;
	}

	bool Network::removeNode(int num)
	{
		if (num < 0 || num >= m_Nodes.size())
			return false;

		m_Nodes.erase(m_Nodes.begin() + num);
		return true;
	}

	const std::string& Network::getName(int num) const
	{
		if (num < 0 || num >= m_Nodes.size())
			return c_EmptyString;
		return m_Nodes[num].m_Name;
	}

	bool Network::setName(int num, const std::string& name)
	{
		if (num < 0 || num >= m_Nodes.size() || name.empty())
			return false;

		auto it = std::find_if(m_Nodes.begin(), m_Nodes.end(), [&](const Node& n) { return name == n.m_Name; });
		if (it != m_Nodes.end() && std::distance(m_Nodes.begin(), it) != num)
			return false;

		m_Nodes[num].m_Name = name;
		return true;
	}

	const std::string& Network::getIp(int num) const
	{
		if (num < 0 || num >= m_Nodes.size())
			return c_EmptyString;
		return m_Nodes[num].m_Ip;
	}

	bool Network::setIp(int num, const std::string& ip)
	{
		if (num < 0 || num >= m_Nodes.size() || ip.empty())
			return false;

		auto it = std::find_if(m_Nodes.begin(), m_Nodes.end(), [&](const Node& n) { return ip == n.m_Ip; });
		if (it != m_Nodes.end() && std::distance(m_Nodes.begin(), it) != num)
			return false;

		m_Nodes[num].m_Ip = ip;
		return true;
	}

	int64_t Network::getId(int num) const
	{
		if (num < 0 || num >= m_Nodes.size())
			return -1;
		return m_Nodes[num].m_Id;
	}

	const std::string& Network::findName(int64_t id) const
	{
		auto it = std::find_if(m_Nodes.begin(), m_Nodes.end(), [&](const Node& n) { return id == n.m_Id; });
		return it != m_Nodes.end() ? it->m_Name : c_NotFoundString;
	}

	const std::string& Network::findIp(int64_t id) const
	{
		auto it = std::find_if(m_Nodes.begin(), m_Nodes.end(), [&](const Node& n) { return id == n.m_Id; });
		return it != m_Nodes.end() ? it->m_Ip : c_EmptyString;
	}

}
