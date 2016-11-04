#include "ProcessServer.h"
#include "ConnectionManager.h"
#include "KillCommand.h"
#include "StopCommand.h"
#include "RaportFinishedCommand.h"

#include "config/Network.h"

#include <QtCore/QProcess>
#include <QtCore/QThread>

#include <iostream>
#include <algorithm>

namespace Network
{
	ProcessServer::ProcessServer(QObject *parent)
		: QObject(parent)
	{
		m_ConnectionManager = new ConnectionManager();
		connect(m_ConnectionManager, &ConnectionManager::connectionSuccessSignal, this, &ProcessServer::onConnect);
		connect(m_ConnectionManager, &ConnectionManager::receivedRaportFnishedSignal, this, &ProcessServer::onFinished, Qt::DirectConnection);
	}
	
	ProcessServer::~ProcessServer()
	{
		delete m_ConnectionManager;
	}
	
	template<class CmdType>
	void Network::ProcessServer::sendTerminateCommand(const std::string& tag, const std::string& ip)
	{
		if (!ip.empty())
		{
			auto it = m_Connections.find(ip);
			if (it == m_Connections.end())
				return;

			if (it->second.processes.find(tag) != it->second.processes.end())
				sendCommand(ip, std::make_shared<CmdType>(ip, tag));
		}
		else
		{
			for (auto connection : m_Connections)
			{
				if (connection.second.processes.find(tag) != connection.second.processes.end())
					sendCommand(connection.first, std::make_shared<CmdType>(connection.first, tag));
			}
		}
	}

	void ProcessServer::runProcess(const std::string& ip, const std::string& command, const std::string& tag)
	{
		const std::string& remoteIp = checkIfLocal(ip);
		std::cout << "Starting process:" << std::endl << command << std::endl;
		sendCommand(remoteIp, std::make_shared<StartCommand>(remoteIp, tag, command));

		m_Connections[remoteIp].processes.insert(tag);
	}
	
	void ProcessServer::stopProcess(const std::string& tag, const std::string& ip /*= ""*/)
	{
		sendTerminateCommand<StopCommand>(tag, ip);
	}

	void ProcessServer::killProcess(const std::string& tag, const std::string& ip /*= ""*/)
	{
		sendTerminateCommand<KillCommand>(tag, ip);
	}

	void ProcessServer::killAll()
	{
		for (auto conEntry : m_Connections)
		{
			const std::string& ip = conEntry.first;
			if (!m_ConnectionManager->isConnected(ip))
				continue;

			const auto& proc = conEntry.second.processes;
			std::set<std::string> tags(proc.begin(), proc.end());
			for (const std::string& tag : tags)
				m_ConnectionManager->sendCommand(std::make_shared<KillCommand>(ip, tag));
		}
	}
	
	int ProcessServer::getNumProcessess() const
	{
		int numProc = 0;
		for (auto proc : m_Connections)
			numProc += (int)proc.second.processes.size();
		
		return numProc;
	}
	
	int ProcessServer::getNumProcessess(const std::string& tag) const
	{
		auto it = m_Connections.find(tag);
		if (it != m_Connections.end())
			return (int)it->second.processes.size();
	
		return 0;
	}

	void ProcessServer::onConnect(QString ip, int port)
	{
		std::lock_guard<std::mutex> lock(m_CommandMutex);
		
		CmdQueue& commands = m_Connections[ip.toStdString()].pendingCommands;
		while (!commands.empty())
		{
			m_ConnectionManager->sendCommand(commands.front());
			commands.pop();
		}
	}

	void ProcessServer::onFinished(RaportFinishedCommand raport, QString ip)
	{
		const std::string& tag = raport.getTag();

		auto connecton = m_Connections.find(ip.toStdString());
		if (connecton == m_Connections.end())
			return;
		
		ProcList& procList = connecton->second.processes;
		auto process = procList.find(tag);
		if (process == procList.end())
			return;

		procList.erase(process);

		emit processFinished(ip, tag.c_str(), raport.exitCode(), raport.exitStatus());
	}

	const std::string& ProcessServer::checkIfLocal(const std::string& ip) const
	{
		if (ip == "127.0.0.1" || ip == "0.0.0.0")
			return m_ConnectionManager->getLocalIp();
		return ip;
	}

	void ProcessServer::sendCommand(const std::string& ip, const std::shared_ptr<AbstractCommand>& command)
	{
		if (m_Connections.find(ip) == m_Connections.end())
		{
			m_ConnectionManager->createConnection(ip, config::Network::getInstance().getTcpPort());
			m_Connections.insert(std::pair<std::string, Connection>(ip, Connection()));
		}

		std::lock_guard<std::mutex> lock(m_CommandMutex);
		if (m_ConnectionManager->isConnected(ip))
			m_ConnectionManager->sendCommand(command);
		else
			m_Connections[ip].pendingCommands.push(command);
	}
}
