#include "TrainManager.h"

#include "config/Action.h"
#include "utils/Filesystem.h"

#include <QtCore/QDir>
#include <QtGui/QDesktopServices>
#include <QtCore/QDateTime>

TrainManager::TrainManager(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	config::Action::getInstance().load();
	m_State = DATABASE;

	connect(this, SIGNAL(error()), this, SLOT(stopAll()), Qt::QueuedConnection);
}

TrainManager::~TrainManager()
{
	stopAll();
}

void TrainManager::processError(QProcess::ProcessError err)
{
	setTrainingFinished();
	printError(static_cast<QProcess*>(sender()));
	log("Training failed.");

	emit error();
}

void TrainManager::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (exitCode != 0 || exitStatus != QProcess::ExitStatus::NormalExit)
	{
		printError(static_cast<QProcess*>(sender()));

		stopAll();
		log("Training error! Stopping.");
		setTrainingFinished();
		return;
	}

	for (QProcess* process : m_ProcessList)
	{
		if (process->state() != QProcess::NotRunning)
			return;
	}

	switch (m_State)
	{
	case TrainState::DATABASE:
		log("Finished generating database.");
		ui.m_CbDatabase->setCheckState(Qt::Checked);
		break;
	case TrainState::GMM:
		log("Finished computing GMM.");
		ui.m_CbGmm->setCheckState(Qt::Checked);
		break;
	case TrainState::FV:
		log("Finished computing Fisher Vectors.");
		ui.m_CbFv->setCheckState(Qt::Checked);
		break;
	case TrainState::SPLIT:
		log("Finished splitting data.");
		ui.m_CbSplit->setCheckState(Qt::Checked);
		break;
	case TrainState::MERGE:
		log("Finished merging data.");
		ui.m_CbMerge->setCheckState(Qt::Checked);
		break;
	case TrainState::SVM:
		log("Finished SVM training.");
		ui.m_CbSvm->setCheckState(Qt::Checked);
		break;
	}

	for (QProcess* process : m_ProcessList)
		delete process;
	m_ProcessList.clear();

	if (m_State == TrainState::DATABASE)
		QDesktopServices::openUrl(QUrl::fromLocalFile((utils::Filesystem::getDataPath() + "action/database.txt").c_str()));

	if (m_State == TrainState::SVM)
	{
		setTrainingFinished();
		QDesktopServices::openUrl(QUrl::fromLocalFile((utils::Filesystem::getDataPath() + "action/svm/results.txt").c_str()));
	}
	else
	{
		m_State = (TrainState)(m_State + 1);
		startProcess();
	}
}

void TrainManager::cancel(QAbstractButton* button)
{
	QApplication::quit();
}

void TrainManager::startProcess()
{
	config::Action::DescriptorType descType = config::Action::getInstance().getDescriptorType();

	switch (m_State)
	{
	case TrainState::DATABASE:
	{
		m_ProcessList.append(new QProcess(this));
		m_ProcessList[0]->setProgram("DatabaseBuilder.exe");

		ui.m_CbDatabase->setCheckState(Qt::PartiallyChecked);

		break;
	}

	case TrainState::GMM:
	{
		if (descType & config::Action::DescriptorType::GBH)
		{
			m_ProcessList.append(new QProcess(this));
			m_ProcessList[0]->setProgram("GetGmmGBH.exe");
		}
		if (descType & config::Action::DescriptorType::MBH)
		{
			m_ProcessList.append(new QProcess(this));
			m_ProcessList[1]->setProgram("GetGmmMBH.exe");
		}
		ui.m_CbGmm->setCheckState(Qt::PartiallyChecked);

		break;
	}

	case TrainState::FV:
	{
		if (descType & config::Action::DescriptorType::GBH)
		{
			m_ProcessList.append(new QProcess(this));
			m_ProcessList[0]->setProgram("GetTrainTestDataGBH.exe");
		}
		if (descType & config::Action::DescriptorType::MBH)
		{
			m_ProcessList.append(new QProcess(this));
			m_ProcessList[1]->setProgram("GetTrainTestDataMBH.exe");
		}
		ui.m_CbFv->setCheckState(Qt::PartiallyChecked);

		break;
	}

	case TrainState::SPLIT:
	{
		m_ProcessList.append(new QProcess(this));
		m_ProcessList[0]->setProgram("SplitData.exe");

		ui.m_CbSplit->setCheckState(Qt::PartiallyChecked);

		break;
	}

	case TrainState::MERGE:
	{
		if (config::Action::getInstance().getDescriptorType() != config::Action::GBH_MBH)
		{
			processFinished(0, QProcess::ExitStatus::NormalExit);
			return;
		}

		m_ProcessList.append(new QProcess(this));
		m_ProcessList[0]->setProgram("MergeData.exe");

		ui.m_CbMerge->setCheckState(Qt::PartiallyChecked);

		break;
	}

	case TrainState::SVM:
	{
		m_ProcessList.append(new QProcess(this));
		m_ProcessList[0]->setProgram("SvmTrain.exe");

		ui.m_CbSvm->setCheckState(Qt::PartiallyChecked);

		break;
	}
	}

	for (QProcess* process : m_ProcessList)
	{
		QString message = "started: " + process->program();
		for (QString arg : process->arguments())
			message += " " + arg;
		log(message);

		connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
		connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
		process->start();
		process->waitForStarted();
	}
}

void TrainManager::clean()
{
	QString dataPath = (utils::Filesystem::getDataPath() + "action/").c_str();
	QDir dir(dataPath + "gbh");
	dir.removeRecursively();

	dir = QDir(dataPath + "mbh");
	dir.removeRecursively();

	dir = QDir(dataPath + "mergedData");
	dir.removeRecursively();

	dir = QDir(dataPath + "svm");
	dir.removeRecursively();
}

void TrainManager::printError(QProcess* process)
{
	QString msg;
	msg += "Error in process: " + process->program() + ". Output:\n";
	msg += process->readAllStandardError();
	log(msg);
}

void TrainManager::log(const QString& message)
{
	ui.m_Console->appendPlainText('[' + QTime::currentTime().toString() + "] " + message);
}

void TrainManager::stopAll()
{
	for (QProcess* process : m_ProcessList)
	{
		process->disconnect(this);
		process->kill();
		process->waitForFinished();
	}
	m_ProcessList.clear();
}

void TrainManager::setTrainingFinished()
{
	ui.m_Buttons->setStandardButtons(QDialogButtonBox::Close);
	QApplication::alert(this);
}

void TrainManager::showEvent(QShowEvent *)
{
	clean();
	startProcess();
}
