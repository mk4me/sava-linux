#include "Aquisition.h"
#include "AquisitionWizard.h"

#include "config/Network.h"
#include "config/Process.h"
#include "config/Camera.h"
#include "config/Milestone.h"
#include "config/Directory.h"
#include "utils/Spawning.h"

#include <QtWidgets/QMessageBox>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QPushButton>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "boost/date_time/posix_time/time_formatters_limited.hpp"

Aquisition::Aquisition(QWidget *parent)
	: QDialog(parent)
	, m_Status(RECORD)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);

	m_CancelButton = ui.m_Buttons->button(QDialogButtonBox::Cancel);
	m_CancelButton->setText("Stop");
	connect(m_CancelButton, &QPushButton::clicked, this, &Aquisition::stopRecord);

	m_AbortButton = ui.m_Buttons->button(QDialogButtonBox::Abort);
	m_AbortButton->setVisible(false);
	connect(m_AbortButton, &QPushButton::clicked, this, &Aquisition::killPipe);

	connect(&m_ProcessServer, &Network::ProcessServer::processFinished, this, &Aquisition::onProcessFinished);

	m_Wizard = new AquisitionWizard(this);

	config::Network::getInstance().load();
	config::Process::getInstance().load();
	config::Camera::getInstance().load();
	config::Milestone::getInstance().load();
	config::Directory::getInstance().load();
}

Aquisition::~Aquisition()
{

}

int Aquisition::exec()
{
	m_Wizard->load("aquisition_wizard.cfg");
	if (m_Wizard->exec() == QWizard::Rejected)
		return QDialog::Rejected;

	m_Wizard->save("aquisition_wizard.cfg");

	if (!startAll())
		return QDialog::Rejected;
	
	return QDialog::exec();
}

void Aquisition::stopRecord()
{
	m_Status = Status::FINISH;

	m_CancelButton->setVisible(false);
	m_AbortButton->setVisible(true);

	m_ProcessServer.killProcess("record");
	ui.m_Text->setText("Waiting for processes to finish..");
}

void Aquisition::killPipe()
{
	if (QMessageBox::question(this, "Confirmation", "Are you sure you want to cancel all operations?") == QMessageBox::Yes)
		m_ProcessServer.killAll();

	QDialog::reject();
}

void Aquisition::onProcessFinished(const QString& ip, const QString& tag, int exitCode, int exitStatus)
{
	if (m_Status != Status::FINISH)
		return;

	if (m_ProcessServer.getNumProcessess() == 0)
		QDialog::accept();
}

bool Aquisition::startAll()
{
	RecordOptionsPage* recordOptions = m_Wizard->getRecordOptionsPage();
	
	if (!recordVideo())
		return false;

	if (recordOptions->getCompressVideo())
	{
		if (!compressVideo())
			return false;
	}

	if (recordOptions->getDetectObjects())
	{
		if (!detectObjects())
			return false;
	}

	if (recordOptions->getPredictActions())
	{
		if (!predictActions())
			return false;
	}

	return true;
}

bool Aquisition::recordVideo()
{
	std::string ip;
	auto nodes = config::Process::getInstance().getAquisitionNodes();
	if (nodes.size() > 0)
		ip = config::Network::getInstance().findIp(nodes[0]);

	if (ip.empty())
	{
		QMessageBox::critical(this, "Error", "There is no computer defined for acquisition process.\nPlease, check \"Process\" configuration.");
		return false;
	}

	m_ProcessServer.runProcess(
		ip,
		m_Wizard->getCameraTypePage()->getCameraType() == CameraTypePage::AXIS ? getAxisCommand() : getMilestoneCommand(),
		"record");

	return true;
}

bool Aquisition::compressVideo()
{
	std::string command = utils::Spawning::getProcessFilePath("Compression");
	command += " --if \"" + m_OutDir + "raw/\"";
	command += " --of \"" + m_OutDir + '\"';
	command += " --timeout " + std::to_string(config::Process::getInstance().getIdleTimeout());

	return runMultiNode("compression", command, "compress", config::Process::getInstance().getCompressionNodes());
}

bool Aquisition::detectObjects()
{
	if (!runMultiNode("path detection", getDetectPathsCommand(), "paths", config::Process::getInstance().getPathDetectionNodes()))
		return false;
	return detectClusters();
}

bool Aquisition::predictActions()
{
	if (!runMultiNode("GBH compute", getComputeGbhCommand(), "gbh", config::Process::getInstance().getGbhNodes()))
		return false;

	if (!runMultiNode("MBH compute", getComputeMbhCommand(), "mbh", config::Process::getInstance().getMbhNodes()))
		return false;

	return runMultiNode("action recognition", getRecognizeActionsCommand(), "svm", config::Process::getInstance().getSvmNodes());
}

bool Aquisition::runMultiNode(const QString& description, const std::string& command, const std::string& tag, const std::vector<int64_t>& nodes)
{
	if (nodes.size() == 0)
	{
		QMessageBox::critical(this, "Error", "There are no computers defined for " + description + " process.\nPlease, check \"Process\" configuration.");
		return false;
	}

	for (auto node : nodes)
	{
		std::string ip = config::Network::getInstance().findIp(node);
		if (ip.empty())
		{
			QMessageBox::critical(this, "Error", "There is no computer defined for " + description + " process.\nPlease, check \"Process\" configuration.");
			return false;
		}

		m_ProcessServer.runProcess(ip, command, tag);
	}

	return true;
}

std::string Aquisition::getDetectPathsCommand() const
{
	std::string command = utils::Spawning::getProcessFilePath("PathDetection");
	command += " --if \"" + m_OutDir + '\"';
	command += " --of \"" + m_OutDir + "path/\"";
	command += " --timeout " + std::to_string(config::Process::getInstance().getIdleTimeout());
	command += " --mask " + getCameraName() + '\"';

	return command;
}

bool Aquisition::detectClusters()
{
	std::string ip;
	auto nodes = config::Process::getInstance().getPathAnalysisNodes();
	if (nodes.size() > 0)
		ip = config::Network::getInstance().findIp(nodes[0]);

	if (ip.empty())
	{
		QMessageBox::critical(this, "Error", "There is no computer defined for path analysis process.\nPlease, check \"Process\" configuration.");
		return false;
	}

	std::string command = utils::Spawning::getProcessFilePath("PathAnalysis");
	command += " --if \"" + m_OutDir + "path/\"";
	command += " --of \"" + m_OutDir + '\"';
	command += " --timeout " + std::to_string(config::Process::getInstance().getIdleTimeout());

	m_ProcessServer.runProcess(ip, command, "clusters");

	return true;
}

std::string Aquisition::getComputeGbhCommand() const
{
	std::string command = utils::Spawning::getProcessFilePath("GbhPipe");
	command += " --if \"" + m_OutDir + '\"';
	command += " --of \"" + m_OutDir + "fv/\"";
	command += " --timeout " + std::to_string(config::Process::getInstance().getIdleTimeout());

	return command;
}

std::string Aquisition::getComputeMbhCommand() const
{
	std::string command = utils::Spawning::getProcessFilePath("MbhPipe");
	command += " --if \"" + m_OutDir + '\"';
	command += " --of \"" + m_OutDir + "fv/\"";
	command += " --timeout " + std::to_string(config::Process::getInstance().getIdleTimeout());

	return command;
}

std::string Aquisition::getRecognizeActionsCommand() const
{
	std::string command = utils::Spawning::getProcessFilePath("SvmPipe");
	command += " --if \"" + m_OutDir + "fv/\"";
	command += " --of \"" + m_OutDir + '\"';
	command += " --timeout " + std::to_string(config::Process::getInstance().getIdleTimeout());

	return command;
}

std::string Aquisition::getDateString()
{
	boost::posix_time::ptime currentTime = boost::posix_time::second_clock::local_time();
	std::string time = to_iso_extended_string(currentTime);
	time[10] = '_';
	time[13] = '-';
	time[16] = '-';
	return time;
}

std::string Aquisition::getMilestoneCommand()
{
	config::Milestone& milestoneConfig = config::Milestone::getInstance();

	std::string outDir = config::Directory::getInstance().getVideosPath();
	outDir += getDateString() + '_';
	outDir += m_Wizard->getMilestoneCameraPage()->getCameraName() + '/';
	m_OutDir = outDir;

	std::string command = utils::Spawning::getProcessFilePath("Aquisition");
	command += " --of \"" + outDir + "raw/\"";
	command += " --in milestone";
	command += " --user " + milestoneConfig.getUser();
	command += " --pass " + milestoneConfig.getPassword();
	command += " --ip " + milestoneConfig.getIp();
	command += " --guid " + m_Wizard->getMilestoneCameraPage()->getCameraGuid();

	return command;
}

std::string Aquisition::getAxisCommand()
{
	config::Camera& cameraConfig = config::Camera::getInstance();
	int cameraIndex = m_Wizard->getAxisCameraPage()->getCameraIndex();

	std::string outDir = config::Directory::getInstance().getVideosPath();
	outDir += getDateString() + '_';
	outDir += cameraConfig.getName(cameraIndex) + '/';
	m_OutDir = outDir;

	std::string command = utils::Spawning::getProcessFilePath("Aquisition");
	command += " --of \"" + outDir + "raw/\"";
	command += " --user " + cameraConfig.getUser(cameraIndex);
	command += " --pass " + cameraConfig.getPassword(cameraIndex);
	command += " --ip " + cameraConfig.getIp(cameraIndex);

	return command;
}

std::string Aquisition::getCameraName() const
{
	if (m_Wizard->getCameraTypePage()->getCameraType() == CameraTypePage::MILESTONE)
		return m_Wizard->getMilestoneCameraPage()->getCameraName();

	int cameraIndex = m_Wizard->getAxisCameraPage()->getCameraIndex();
	return config::Camera::getInstance().getName(cameraIndex);
}

void Aquisition::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::question(this, "Confirmation", "Are you sure you want to stop acquisition?") == QMessageBox::Yes)
		event->accept();
	else
		event->ignore();
}
