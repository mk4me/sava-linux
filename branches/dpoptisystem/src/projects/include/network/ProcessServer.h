#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QtCore/QObject>

#include "RaportFinishedCommand.h"

#include <queue>
#include <mutex>
#include <memory>
#include <set>

namespace Network
{
	class ConnectionManager;
	class AbstractCommand;

	class ProcessServer : public QObject
	{
		Q_OBJECT
	
	public:
		ProcessServer(QObject *parent = nullptr);
		~ProcessServer();
	
		void runProcess(const std::string& ip, const std::string& command, const std::string& tag);
		void stopProcess(const std::string& tag, const std::string& ip = "");
		void killProcess(const std::string& tag, const std::string& ip = "");

		void killAll();
	
		int getNumProcessess() const;
		int getNumProcessess(const std::string& tag) const;
	
	signals:
		void processFinished(const QString& ip, const QString& tag, int exitCode, int exitStatus);
		void procsssError(const QString& ip, const QString& tag, int error);
	
	private slots:
		void onConnect(QString ip, int port);
		void onFinished(RaportFinishedCommand raport, QString ip);

	private:
		typedef std::queue<std::shared_ptr<AbstractCommand>> CmdQueue;
		typedef std::multiset<std::string> ProcList;
		
		struct Connection
		{
			ProcList processes;
			CmdQueue pendingCommands;
		};
		std::map<std::string, Connection> m_Connections;

		ConnectionManager* m_ConnectionManager;
		std::mutex m_CommandMutex;

		const std::string& checkIfLocal(const std::string& ip) const;
		void sendCommand(const std::string& ip, const std::shared_ptr<AbstractCommand>& command);

		template<class CmdType>
		void sendTerminateCommand(const std::string& tag, const std::string& ip);
	};
}

#endif // PROCESSMANAGER_H
