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
		//! Destructor.
		~ProcessManagerLib();

		//! Starts accepting connection to local endpoint.
		/*!
		\param _localIp of endpoint.
		\param _localPort of endpoint.
		*/
		void start(const std::string& _localIp, const int _localPort);
		//! Starts accepting connection to local endpoint.
		/*!
		\param _localPort of endpoint.
		*/
		void start(const int _localPort);

		//! Sends message through network.
		/*!
		\param _message that will be send.
		*/
		void send(const std::string& _message);
		//! Sends command through network.
		/*!
		\param _abstractCommand that will be send.
		*/
		void sendCommand(std::shared_ptr<AbstractCommand> _abstractCommand);		
		
	private:
		//! Connects all signals and slots for this class.
		void init();

		//! Creates new process.
		/*!
		\param _exePathWithArgs executive path with arguments.
		\return shared pointer to new created process.
		*/
		std::shared_ptr<QProcess> createNewProcess(const std::string& _exePathWithArgs);
		//! Starts process.
		/*!
		\param _abstractCommand that describe process.
		*/
		void start(std::shared_ptr<AbstractCommand> _abstractCommand);
		//! Stop process.
		/*!
		\param _abstractCommand that describe process.
		*/
		void stop(std::shared_ptr<AbstractCommand> _abstractCommand);
		//! Kill process.
		/*!
		\param _abstractCommand that describe process.
		*/
		void kill(std::shared_ptr<AbstractCommand> _abstractCommand);

		//! Get process from internal map.
		/*!
		\param _qObject from we search process.
		\param _tag search process by tag.
		\param _killedByCommand tries to get process that was killed by command from network.
		\param _killedByDisconnection tries to get process that was killed by disconnection.
		\return shared pointer for process from internal map.
		*/
		std::shared_ptr<QProcess> getProcessFrom(QObject* _qObject, std::string& _tag, bool& _killedByCommand, bool& _killedByDisconnection);
		//! Erase process from internal map.
		/*!
		\param _process from that will be removed.
		*/
		void eraseProcessFromProcessesList(std::shared_ptr<QProcess> _process);
		//! Get executive path and args from process from internal map.
		/*!
		\param _process from which we want information.
		\return executive path and args.
		*/
		std::string getExePathWithArgsFrom(std::shared_ptr<QProcess> _process);

		//! Network component that accepts connections.
		Acceptor acceptor_;

		//! Internal process with special parameters .
		class Processes
		{
		public:
			//! List of shared pointers for QProcess.
			std::list<std::shared_ptr<QProcess>> procList_;
			//! Was process killed by command?
			bool killedByCommand_ = false;
			//! Was process killed by network disconnection?
			bool killedByDisconnection_ = false;
		};
		//! Map of processes with tags
		std::unordered_map <std::string, std::shared_ptr<Processes>> processes_;

		//! Before we finish destructor all processes must be killed.
		bool allQProcKilled;

	private slots:
		//! Decides what to do with message from network.
		/*!
		\param _message that came from network.	
		*/
		void handleReceiveMessage(const QString& _message);
		//! Decides what to do when process error came.
		/*!
		\param _error type from QProcess.
		*/
		void handleError(QProcess::ProcessError _error);
		//! Decides what to do when process finished.
		/*!
		\param _exitCode when process finished.
		\param _exitStatus when process finished.
		*/
		void handleFinished(int _exitCode, QProcess::ExitStatus _exitStatus);
		//! Starts process.
		/*!
		\param _abstractCommand that describe process.
		*/
		void startInvoked(std::shared_ptr<AbstractCommand> _abstractCommand);
		//! Invoked method executed from destructor, destroys all processes.
		void killAllQProcInvoked();
		//! Log all events.
		/*!
		\param _log string with message.
		\param _isError was this error?
		*/
		void handleLog(const QString& _log, bool _isError = false);

	signals:
		//! Emits log signal.
		/*!
		\param _log string with message.
		\param _isError was this error?
		*/
		void logSignal(const QString& _log, bool _isError);
		//! Emits signal when new process was created.
		/*!
		\param _newProc new process shared pointer.
		\param _exePathWithArgs executive path with arguments.
		*/
		void createNewProcessSignal(std::shared_ptr<QProcess> _newProc, const QString& _exePathWithArgs);
		//! Emits signal when process finish.
		/*!
		\param _proc process that finished.
		\param _exitCode when finished.
		\param _killedByCommand was finished by command?
		\param _killedByDisconnection was finished by network disconnection?
		*/
		void finishProcessSignal(std::shared_ptr<QProcess> _proc, int _exitCode, bool _killedByCommand, bool _killedByDisconnection);
	};
}

#endif // PROCESS_MANAGER_LIB_H
