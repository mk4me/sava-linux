#include "TrainManager.h"

TrainManager::TrainManager(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_ActionConfig.load();
	m_State = GMM;

	connect(this, SIGNAL(error()), this, SLOT(stopAll()), Qt::QueuedConnection);
}

TrainManager::~TrainManager()
{
	stopAll();
}

void TrainManager::processError(QProcess::ProcessError err)
{
	ui.m_Buttons->setStandardButtons(QDialogButtonBox::Close);
	QProcess* process = static_cast<QProcess*>(sender());
	ui.m_Console->appendPlainText("Process error: " + process->program() + "\nTraining failed.");

	emit error();
}

void TrainManager::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (exitCode != 0 || exitStatus != QProcess::ExitStatus::NormalExit)
	{
		stopAll();
		ui.m_Console->appendPlainText("Training error! Stopping.\n");
		ui.m_Buttons->setStandardButtons(QDialogButtonBox::Close);
		return;
	}

	for (QProcess* process : m_ProcessList)
	{
		if (process->state() != QProcess::NotRunning)
			return;
	}

	switch (m_State)
	{
	case TrainState::GMM:
		ui.m_Console->appendPlainText("Finished computing GMM.\n");
		ui.m_CbGmm->setCheckState(Qt::Checked);
		break;
	case TrainState::FV:
		ui.m_Console->appendPlainText("Finished computing Fisher Vectors.\n");
		ui.m_CbFv->setCheckState(Qt::Checked);
		break;
	case TrainState::SPLIT:
		ui.m_Console->appendPlainText("Finished splitting data.\n");
		ui.m_CbSplit->setCheckState(Qt::Checked);
		break;
	case TrainState::MERGE:
		ui.m_Console->appendPlainText("Finished merging data.\n");
		ui.m_CbMerge->setCheckState(Qt::Checked);
		break;
	case TrainState::SVM:
		ui.m_Console->appendPlainText("Finished SVM training.\n");
		ui.m_CbSvm->setCheckState(Qt::Checked);
		break;
	}

	for (QProcess* process : m_ProcessList)
		delete process;
	m_ProcessList.clear();

	if (m_State == TrainState::SVM)
	{
		ui.m_Buttons->setStandardButtons(QDialogButtonBox::Close);
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
	config::Action::DescriptorType descType = m_ActionConfig.getDescriptorType();

	switch (m_State)
	{
	case TrainState::GMM:
	{
		QStringList args;
		args << m_ActionConfig.getDatabasePath().c_str();
		if (descType & config::Action::DescriptorType::GBH)
		{
			m_ProcessList.append(new QProcess(this));
			m_ProcessList[0]->setProgram("GetGmmGBH.exe");
			m_ProcessList[0]->setArguments(args);
		}
		if (descType & config::Action::DescriptorType::MBH)
		{
			m_ProcessList.append(new QProcess(this));
			m_ProcessList[1]->setProgram("GetGmmMBH.exe");
			m_ProcessList[1]->setArguments(args);
		}
		ui.m_CbGmm->setCheckState(Qt::PartiallyChecked);

		break;
	}

	case TrainState::FV:
	{
		QStringList args;
		args << m_ActionConfig.getDatabasePath().c_str();
		if (descType & config::Action::DescriptorType::GBH)
		{
			m_ProcessList.append(new QProcess(this));
			m_ProcessList[0]->setProgram("GetTrainTestDataGBH.exe");
			m_ProcessList[0]->setArguments(args);
		}
		if (descType & config::Action::DescriptorType::MBH)
		{
			m_ProcessList.append(new QProcess(this));
			m_ProcessList[1]->setProgram("GetTrainTestDataMBH.exe");
			m_ProcessList[1]->setArguments(args);
		}
		ui.m_CbFv->setCheckState(Qt::PartiallyChecked);

		break;
	}

	case TrainState::SPLIT:
	{
		m_ProcessList.append(new QProcess(this));
		m_ProcessList[0]->setProgram("SplitData.exe");
		m_ProcessList[0]->setArguments(QStringList() << m_ActionConfig.getDatabasePath().c_str() << m_ActionConfig.getDescriptorName().c_str());

		ui.m_CbSplit->setCheckState(Qt::PartiallyChecked);

		break;
	}

	case TrainState::MERGE:
	{
		if (m_ActionConfig.getDescriptorType() != config::Action::GBH_MBH)
		{
			processFinished(0, QProcess::ExitStatus::NormalExit);
			return;
		}

		QStringList args;
		args << m_ActionConfig.getDescriptor(config::Action::GBH).path.c_str();
		args << QString("%1").arg(m_ActionConfig.getDescriptor(config::Action::GBH).size);
		args << m_ActionConfig.getDescriptor(config::Action::MBH).path.c_str();
		args << QString("%1").arg(m_ActionConfig.getDescriptor(config::Action::MBH).size);
		m_ProcessList.append(new QProcess(this));
		m_ProcessList[0]->setProgram("MergeData.exe");

		ui.m_CbMerge->setCheckState(Qt::PartiallyChecked);

		break;
	}

	case TrainState::SVM:
	{
		QStringList args;
		args << "-o" << "1";
		args << "-x" << QString("%1").arg(m_ActionConfig.getDescriptorSize());
		args << "-f" << m_ActionConfig.getDescriptorPath().c_str();
		m_ProcessList.append(new QProcess(this));
		m_ProcessList[0]->setProgram("SvmTrain.exe");
		m_ProcessList[0]->setArguments(args);

		ui.m_CbSvm->setCheckState(Qt::PartiallyChecked);

		break;
	}
	}

	for (QProcess* process : m_ProcessList)
	{
		QString message = "started: " + process->program();
		for (QString arg : process->arguments())
			message += " " + arg;
		ui.m_Console->appendPlainText(message);

		connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
		connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
		process->start();
		process->waitForStarted();
	}
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

void TrainManager::showEvent(QShowEvent *)
{
	startProcess();
}
