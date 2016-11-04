#pragma once
#ifndef PROCESS_MANAGER_LIB_H
#define PROCESS_MANAGER_LIB_H

#include <string>
#include <unordered_map>
#include <list>

#include "Acceptor.h"
#include "AbstractCommand.h"

#include <QtCore/QProcess>

namespace Network
{
	class ProcessManagerLib : public QObject
	{
		Q_OBJECT

	public:		
		~ProcessManagerLib();

		void start(const std::string& _localIp, const int _localPort);
		void start(const int _localPort);

		void send(const std::string& _message);
		void sendCommand(std::shared_ptr<AbstractCommand> _abstractCommand);		
		
	private:
		void init();

		std::shared_ptr<QProcess> createNewProcess(const std::string& _exePathWithArgs);
		void start(std::shared_ptr<AbstractCommand> _abstractCommand);
		void stop(std::shared_ptr<AbstractCommand> _abstractCommand);
		void kill(std::shared_ptr<AbstractCommand> _abstractCommand);

		std::shared_ptr<QProcess> getProcessFrom(QObject* _qObject, std::string& _tag, bool& _killedByCommand);
		void eraseProcessFromProcessesList(std::shared_ptr<QProcess> _process);
		std::string getExePathWithArgsFrom(std::shared_ptr<QProcess> _process);

		Acceptor acceptor_;

		//procesy dla danego taga
		class Processes
		{
		public:
			std::list<std::shared_ptr<QProcess>> procList_;
			bool killedByCommand_ = false; //czy proc zabity przez komende?
		};
		std::unordered_map <std::string, std::shared_ptr<Processes>> processes_;

		bool allQProcKilled;

	private slots:
		void handleReceiveMessage(const QString& _message);
		void handleError(QProcess::ProcessError _error);
		void handleFinished(int _exitCode, QProcess::ExitStatus _exitStatus);
		void startInvoked(std::shared_ptr<AbstractCommand> _abstractCommand);
		void killAllQProcInvoked();
		void handleLog(const QString& _log, bool _isError = false);

	signals:
		void logSignal(const QString& _log, bool _isError);
		void createNewProcessSignal(std::shared_ptr<QProcess> _newProc, const QString& _exePathWithArgs);
		void finishProcessSignal(std::shared_ptr<QProcess> _proc, int _exitCode, bool _killedByCommand);
	};
}

#endif // PROCESS_MANAGER_LIB_H
