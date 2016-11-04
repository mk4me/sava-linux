#include "ProcessManagerLib.h"

#include "StopCommand.h"
#include "KillCommand.h"
#include "RaportErrorCommand.h"
#include "RaportFinishedCommand.h"

#include <QtCore/QThread>

namespace Network
{
	ProcessManagerLib::~ProcessManagerLib()
	{
		QMetaObject::invokeMethod(this, "killAllQProcInvoked");

		//trzeba potestowac czy allQProcKilled nie powinno byc zmutexowane
		while (!allQProcKilled)
		{
			//czy przy duzej ilosci procesow zdarzy przeslac raporty o bledach przy ich ubijaniu?
			//czy moze by nalezalo rozlaczyc sygnaly?
			QThread::msleep(100);
		}
	}

	void ProcessManagerLib::start(const std::string& _localIp, const int _localPort)
	{		
		init();
		acceptor_.start(_localIp, _localPort);
	}

	void ProcessManagerLib::start(const int _localPort)
	{
		init();
		acceptor_.start(_localPort);	
	}

	void ProcessManagerLib::send(const std::string& _message)
	{
		acceptor_.send(_message);
	}

	void ProcessManagerLib::sendCommand(std::shared_ptr<AbstractCommand> _abstractCommand)
	{
		acceptor_.send(_abstractCommand->encode());
	}

	void ProcessManagerLib::init()
	{
		handleLog("Process Manager Started");

		allQProcKilled = false;
		qRegisterMetaType<std::shared_ptr<AbstractCommand>>("std::shared_ptr<AbstractCommand>");

		QObject::connect(&acceptor_, SIGNAL(acceptorLogSignal(const QString&, bool)),
			this, SLOT(handleLog(const QString&, bool)), Qt::DirectConnection);

		QObject::connect(&acceptor_, SIGNAL(receivedMessageSignal(const QString&)),
			this, SLOT(handleReceiveMessage(const QString&)), Qt::DirectConnection);
		QObject::connect(&acceptor_, SIGNAL(connectionClosedSignal()),
			this, SLOT(killAllQProcInvoked()), Qt::DirectConnection);
	}

	std::shared_ptr<QProcess> ProcessManagerLib::createNewProcess(const std::string& _exePathWithArgs)
	{
		allQProcKilled = false;

		std::shared_ptr<QProcess> newProc = std::make_shared<QProcess>();	
		emit createNewProcessSignal(newProc, QString::fromStdString(_exePathWithArgs));

		QObject::connect(newProc.get(), SIGNAL(error(QProcess::ProcessError)),
			this, SLOT(handleError(QProcess::ProcessError)), Qt::DirectConnection);
		QObject::connect(newProc.get(), SIGNAL(finished(int, QProcess::ExitStatus)),
			this, SLOT(handleFinished(int, QProcess::ExitStatus)), Qt::DirectConnection);

		newProc->start(QString::fromStdString(_exePathWithArgs));
		return newProc;
	}

	void ProcessManagerLib::startInvoked(std::shared_ptr<AbstractCommand> _abstractCommand)
	{
		std::shared_ptr<StartCommand> startCommand = std::dynamic_pointer_cast<StartCommand>(_abstractCommand);

		auto tagFound = processes_.find(startCommand->getTag());

		//not found
		if (tagFound == processes_.end())
		{
			std::shared_ptr<Processes> newProcesses = std::make_shared<Processes>();
			newProcesses->procList_.push_back(createNewProcess(startCommand->exePathWithArgs()));
 
			processes_.insert(std::pair<std::string, std::shared_ptr<Processes>>(startCommand->getTag(), newProcesses));
		}
		//found
		else
		{
			tagFound->second->procList_.push_back(createNewProcess(startCommand->exePathWithArgs()));
		}
	}

	void ProcessManagerLib::start(std::shared_ptr<AbstractCommand> _abstractCommand)
	{		
		QMetaObject::invokeMethod(this, "startInvoked", Q_ARG(std::shared_ptr<AbstractCommand>, _abstractCommand));		
	}

	void ProcessManagerLib::stop(std::shared_ptr<AbstractCommand> _abstractCommand)
	{
		std::shared_ptr<StopCommand> stopCommand = std::dynamic_pointer_cast<StopCommand>(_abstractCommand);

 		auto tagFound = processes_.find(stopCommand->getTag());

		//not found
		if (tagFound == processes_.end())
		{
			//do nothing
			return;
		}
		//found
		else
		{
			auto taggedProcceses = tagFound->second;
			taggedProcceses->killedByCommand_ = true;
			for (auto it = taggedProcceses->procList_.begin(); it != taggedProcceses->procList_.end(); ++it)
				(*it)->terminate();
		}
	}

	void ProcessManagerLib::kill(std::shared_ptr<AbstractCommand> _abstractCommand)
	{
		std::shared_ptr<KillCommand> killCommand = std::dynamic_pointer_cast<KillCommand>(_abstractCommand);
		
		auto tagFound = processes_.find(killCommand->getTag());

		//not found
		if (tagFound == processes_.end())
		{
			//do nothing
			return;
		}
		//found
		else
		{
			auto taggedProcceses = tagFound->second;
			taggedProcceses->killedByCommand_ = true;
			for (auto it = taggedProcceses->procList_.begin(); it != taggedProcceses->procList_.end(); ++it)
				(*it)->kill();
		}
	}

	void ProcessManagerLib::handleReceiveMessage(const QString& _message)
	{
		auto receivedCommand = AbstractCommand::decode(_message.toStdString());
		if (receivedCommand == nullptr)
			return;

		if (!receivedCommand->isThisForMe(acceptor_.getLocalIp()))
		{
			handleLog("^^^^^^ this message was not for me!");
			return;
		}

		handleLog("RECEIVED: " +_message);

		switch (receivedCommand->getType())
		{
		case AbstractCommand::Start:
			start(receivedCommand);			
			break;
		case AbstractCommand::Stop:
			stop(receivedCommand);
			break;
		case AbstractCommand::Kill:
			kill(receivedCommand);
			break;
		default:
			break;
		}
	}

	void ProcessManagerLib::handleError(QProcess::ProcessError _error)
	{
		std::string tag;
		bool killedByCommand = false;
		std::shared_ptr<QProcess> process = getProcessFrom(QObject::sender(), tag, killedByCommand);
		if (process == nullptr || process.get() == nullptr)
			return;

		std::string exePathWithArgs = getExePathWithArgsFrom(process);

		std::shared_ptr<RaportErrorCommand> raport
			= std::make_shared<RaportErrorCommand>(acceptor_.getRemoteIp(), tag, exePathWithArgs, _error);

		sendCommand(raport);
	}

	void ProcessManagerLib::handleFinished(int _exitCode, QProcess::ExitStatus _exitStatus)
	{
		std::string tag;
		bool killedByCommand = false;
		std::shared_ptr<QProcess> process = getProcessFrom(QObject::sender(), tag, killedByCommand);
		if (process == nullptr || process.get() == nullptr)
			return;

		emit finishProcessSignal(process, _exitCode, killedByCommand);

		eraseProcessFromProcessesList(process);
		std::string exePathWithArgs = getExePathWithArgsFrom(process);

		std::shared_ptr<RaportFinishedCommand> raport
			= std::make_shared<RaportFinishedCommand>(acceptor_.getRemoteIp(), tag, exePathWithArgs, _exitCode, _exitStatus);

		sendCommand(raport);		
	}

	std::shared_ptr<QProcess> ProcessManagerLib::getProcessFrom(QObject* _qObject, std::string& _tag, bool& _killedByCommand)
	{
		if (_qObject == nullptr)
			return nullptr;

		bool found = false;
		std::shared_ptr<QProcess> processFound;

		for (auto it = processes_.begin(); it != processes_.end(); ++it)
		{			
			auto taggedProcceses = it->second;
			for (auto taggedIt = taggedProcceses->procList_.begin(); taggedIt != taggedProcceses->procList_.end(); ++taggedIt)
			{
				if ((*taggedIt).get() == _qObject)
				{
					_tag = it->first;
					_killedByCommand = taggedProcceses->killedByCommand_;
					processFound = (*taggedIt);
					found = true;
					break;
				}
			}

			if (found)
				break;
		}

		return processFound;
	}

	void ProcessManagerLib::eraseProcessFromProcessesList(std::shared_ptr<QProcess> _process)
	{
		for (auto it = processes_.begin(); it != processes_.end(); ++it)
		{
			auto taggedProcceses = it->second;
			for (auto taggedIt = taggedProcceses->procList_.begin(); taggedIt != taggedProcceses->procList_.end();)
				if (*taggedIt == _process)
					taggedIt = taggedProcceses->procList_.erase(taggedIt);
				else
					++taggedIt;
		}

		//wykasuj zerowe kontenery
		for (auto it = processes_.begin(); it != processes_.end();)
			if (it->second->procList_.size() == 0)
				it = processes_.erase(it);
			else
				++it;
	}

	std::string ProcessManagerLib::getExePathWithArgsFrom(std::shared_ptr<QProcess> _process)
	{
		std::string exePathWithArgs = _process->program().toStdString();
		auto args = _process->arguments().toStdList();
		for (auto it = args.begin(); it != args.end(); ++it)
			exePathWithArgs += " " + it->toStdString();

		return exePathWithArgs;
	}

	void ProcessManagerLib::killAllQProcInvoked()
	{
		for (auto it = processes_.begin(); it != processes_.end(); ++it)
		{
			auto taggedProcceses = it->second;
			for (auto taggedIt = taggedProcceses->procList_.begin(); taggedIt != taggedProcceses->procList_.end(); ++taggedIt)
			{
				(*taggedIt)->kill();
			}
		}

		allQProcKilled = true;
	}

	void ProcessManagerLib::handleLog(const QString& _log, bool _isError)
	{
		emit logSignal(_log, _isError);

		//zostawiam to na wypadek apki konsolowej
// 		if (_isError)
// 			std::cerr << _log.toStdString() << std::endl;
// 		else
// 			std::cout << _log.toStdString() << std::endl;
	}
}