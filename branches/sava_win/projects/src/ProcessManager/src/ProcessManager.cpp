#include "ProcessManager.h"
#include "ProcessTab.h"
#include <utils/Filesystem.h>
#include <utils/MemoryLog.h>
#include <config/Diagnostic.h>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtNetwork/QHostAddress>

const std::string ProcessManager::logFolderName_ = "process_manager";
const std::string ProcessManager::logFileName_ = "pm.log";

ProcessManager::ProcessManager(QWidget *parent)
	: QMainWindow(parent), logger_(this)
{
	ui.setupUi(this);
	ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->hide(); //chowanie krzy�yka na zerowej zakladce
	procHistory_.clear();
	
	config::Diagnostic::getInstance().load();
	if (config::Diagnostic::getInstance().getLogMemoryUsage())
	{
		new utils::MemoryLog(this);
	}

	//czyszczenie katalogu z logami
	logFolderPath_ = utils::Filesystem::getUserPath() + logFolderName_;
	utils::Filesystem::removeContents(logFolderPath_);
	auto logDirPath = QString::fromStdString(logFolderPath_);
	QDir dir(logDirPath);
	if (!dir.exists()) {
		bool res = dir.mkpath(logDirPath);
		if (!res) {
			std::cerr << "Unable to create: " << logFolderPath_ << std::endl;
		}
	}
	logger_.setFilePath(QString::fromStdString(logFolderPath_ + "/" + logFileName_));

	//dodanie kontrolki logujacej
	QVBoxLayout* layout = (QVBoxLayout*)ui.tabPM->layout();
	layout->insertWidget(0, &logger_);

	//podlaczenie do biblioteki sieciowej
	QObject::connect(&pmLib_, SIGNAL(logSignal(const QString&, bool)),
		this, SLOT(logReceive(const QString&, bool)), Qt::QueuedConnection);
	QObject::connect(&pmLib_, SIGNAL(createNewProcessSignal(std::shared_ptr<QProcess>, const QString&)),
		this, SLOT(handleCreateNewProcess(std::shared_ptr<QProcess>, const QString&)), Qt::DirectConnection);
	QObject::connect(&pmLib_, SIGNAL(finishProcessSignal(std::shared_ptr<QProcess>, int, bool, bool)),
		this, SLOT(handleFinishProcess(std::shared_ptr<QProcess>, int, bool, bool)), Qt::DirectConnection);

	//parse arguments from command line
	config::Network::getInstance().load();
	QStringList appPrams = qApp->arguments();
	if (appPrams.size() > 1)
	{
		QHostAddress myIp;
		if (myIp.setAddress(appPrams.at(1)))
			pmLib_.start(appPrams.at(1).toStdString(), config::Network::getInstance().getTcpPort());
		else
			pmLib_.start(config::Network::getInstance().getTcpPort());
	}
	else
		pmLib_.start(config::Network::getInstance().getTcpPort());

	QObject::connect(ui.lineEditPM, SIGNAL(returnPressed()),
		this, SLOT(handleLineEditPM_ReturnPressed()), Qt::DirectConnection);

	QObject::connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

ProcessManager::~ProcessManager()
{

}

void ProcessManager::logReceive(const QString& _log, bool _isError)
{
	if (_isError)
	{
		logger_.log(_log, Qt::red);
		return;
	}

	logger_.log(_log);
}

void ProcessManager::handleLineEditPM_ReturnPressed()
{
	logger_.log("SENT: " + ui.lineEditPM->text().trimmed(), Qt::blue);
 	pmLib_.send(ui.lineEditPM->text().trimmed().toStdString());
 	ui.lineEditPM->clear();
}

QString getProcName(const QString& pathWithArgs)
{
#ifdef _WIN32
	QStringList pieces = pathWithArgs.split(".exe");
	pieces = pieces[0].split("/");
	pieces = pieces[pieces.length() - 1].split("\\");
	return pieces[0];
#else
    try {
		std::string path = pathWithArgs.toStdString();
		auto part = path.substr(0, path.find(" "));
		auto start = part.rfind("/");
		auto procName = part.substr(start + 1, part.size());
		return QString::fromStdString(procName);
	} catch (...) {
		// on error return App0, App1, App2 ...
		// use memoizing
		static int appCount = 0;
		static std::map<QString, QString> paths;
		auto it = paths.find(pathWithArgs);
		if (it != paths.end()) {
			return it->second;
		} else {
			auto p = QString("App%1").arg(appCount++);
			paths[pathWithArgs] = p;
			return p;
		}
	}
#endif
}


void ProcessManager::handleCreateNewProcess(std::shared_ptr<QProcess> _newProc, const QString& _exePathWithArgs)
{
	//wydzielamy nazwe zakladki
	auto procName = getProcName(_exePathWithArgs);
	
	//sprawdzamy ile razy byl stworzony proces od staru proc mng
	int id = 0;
	if (procHistory_.contains(procName))
	{
		id = procHistory_.value(procName);
		id++;
		procHistory_.insert(procName, id);
	}
	else
		procHistory_.insert(procName, 0);

	//stworz sciezke do loga
	QString processLogFilePath = QString::fromStdString(logFolderPath_) + "/" + procName + "_" + QString::number(id) + ".log";

	//tworzymy nowa zakladke
	ui.tabWidget->insertTab(ui.tabWidget->count(), new ProcessTab(this, _newProc, _exePathWithArgs, processLogFilePath), procName);
	ui.tabWidget->tabBar()->tabButton(ui.tabWidget->count()-1, QTabBar::RightSide)->hide(); //chowanie krzy�yka
}

void ProcessManager::handleFinishProcess(std::shared_ptr<QProcess> _proc, int _exitCode, bool _killedByCommand, bool _killedByDisconnection)
{	
	for (int i = 0; i < ui.tabWidget->count(); ++i)
	{
		ProcessTab* pt = dynamic_cast<ProcessTab*>(ui.tabWidget->widget(i));

		if (pt != nullptr && pt->getProcPtr().get() == _proc.get())
		{
			if (_exitCode == 0 || _killedByCommand || _killedByDisconnection)
			{
				pt->getProcPtr()->disconnect();
				closeTab(i);
			}
			else
			{
				ui.tabWidget->tabBar()->tabButton(i, QTabBar::RightSide)->show();
				pt->WriteExitCode(_exitCode);
			}
			break;
		}
	}
}

void ProcessManager::closeTab(int _index)
{
	auto tab = ui.tabWidget->widget(_index);
	ui.tabWidget->removeTab(_index);
	if (tab)
		delete tab;
}
