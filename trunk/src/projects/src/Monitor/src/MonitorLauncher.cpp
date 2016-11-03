#include "MonitorLauncher.h"

#include "config/Network.h"
#include "config/Process.h"
#include "config/Directory.h"

#include <QMessageBox>

MonitorLauncher::MonitorLauncher(const std::string& ip, const std::string& user, const std::string& password, const std::string& guid, const std::string& mask, QObject *parent)
	: QObject(parent)
	, m_Ip(ip)
	, m_User(user)
	, m_Password(password)
	, m_Guid(guid)
	, m_Mask(mask)
{
	config::Network::getInstance().load();
	config::Process::getInstance().load();
	config::Directory::getInstance().load();
}

MonitorLauncher::~MonitorLauncher()
{
	m_ProcessServer.killAll();
}

bool MonitorLauncher::launchPipe()
{
	if (!startPipe())
	{
		m_ProcessServer.killAll();
		return false;
	}
	
	return true;
}

bool MonitorLauncher::startPipe()
{
	config::Process& procCfg = config::Process::getInstance();

	if (!runCommand("acquisition", m_Guid.empty() ? getAxisCommand() : getMilestoneCommand(), "record", procCfg.getAquisitionNodes()))
		return false;

	if (!runCommand("compression", getCompressVideoCommand(), "compress", procCfg.getCompressionNodes()))
		return false;

	if (!runCommand("path detection", getDetectPathsCommand(), "paths", procCfg.getPathDetectionNodes()))
		return false;

	if (!runCommand("path analysis", getDetectClustersCommand(), "clusters", procCfg.getPathAnalysisNodes()))
		return false;

	if (!runCommand("GBH compute", getComputeGbhCommand(), "gbh", procCfg.getGbhNodes()))
		return false;

	if (!runCommand("MBH compute", getComputeMbhCommand(), "mbh", procCfg.getMbhNodes()))
		return false;

	if (!runCommand("action recognition", getRecognizeActionsCommand(), "svm", procCfg.getSvmNodes()))
		return false;

	return true;
}

bool MonitorLauncher::runCommand(const QString& description, const std::string& command, const std::string& tag, const std::vector<__int64>& nodes)
{
	if (nodes.size() == 0)
	{
		QMessageBox::critical(nullptr, "Error", "There are no computers defined for " + description + " process.\nPlease, check \"Process\" configuration.");
		return false;
	}

	for (auto node : nodes)
	{
		std::string ip = config::Network::getInstance().findIp(node);
		if (ip.empty())
		{
			QMessageBox::critical(nullptr, "Error", "There is no computer defined for " + description + " process.\nPlease, check \"Process\" configuration.");
			return false;
		}

		m_ProcessServer.runProcess(ip, command, tag);
	}

	return true;
}

std::string MonitorLauncher::getCompressVideoCommand() const
{
	std::string command = "Compression.exe";
	command += " --if \"" + config::Directory::getInstance().getTemporaryPath() + "raw/\"";
	command += " --of \"" + config::Directory::getInstance().getTemporaryPath() + '\"';

	return command;
}

std::string MonitorLauncher::getDetectPathsCommand() const
{
	std::string command = "PathDetection.exe";
	command += " --if \"" + config::Directory::getInstance().getTemporaryPath() + '\"';
	command += " --of \"" + config::Directory::getInstance().getTemporaryPath() + "path/\"";
	if(!m_Mask.empty())
		command += " --mask \"" + m_Mask + '\"';

	return command;
}

std::string MonitorLauncher::getDetectClustersCommand() const
{
	std::string command = "PathAnalysis.exe";
	command += " --if \"" + config::Directory::getInstance().getTemporaryPath() + "path/\"";
	command += " --of \"" + config::Directory::getInstance().getTemporaryPath() + '\"';

	return command;
}

std::string MonitorLauncher::getComputeGbhCommand() const
{
	std::string command = "GbhPipe.exe";
	command += " --if \"" + config::Directory::getInstance().getTemporaryPath() + '\"';
	command += " --of \"" + config::Directory::getInstance().getTemporaryPath() + "fv/\"";

	return command;
}

std::string MonitorLauncher::getComputeMbhCommand() const
{
	std::string command = "MbhPipe.exe";
	command += " --if \"" + config::Directory::getInstance().getTemporaryPath() + '\"';
	command += " --of \"" + config::Directory::getInstance().getTemporaryPath() + "fv/\"";
	
	return command;
}

std::string MonitorLauncher::getRecognizeActionsCommand() const
{
	std::string command = "SvmPipe.exe";
	command += " --if \"" + config::Directory::getInstance().getTemporaryPath() + "fv/\"";
	command += " --of \"" + config::Directory::getInstance().getTemporaryPath() + '\"';

	return command;
}

std::string MonitorLauncher::getMilestoneCommand() const
{
	std::string command = "Aquisition.exe";
	command += " --of \"" + config::Directory::getInstance().getTemporaryPath() + "raw/\"";
	command += " --in milestone";
	command += " --user " + m_User;
	command += " --pass " + m_Password;
	command += " --ip " + m_Ip;
	command += " --guid " + m_Guid;

	return command;
}

std::string MonitorLauncher::getAxisCommand() const
{
	std::string command = "Aquisition.exe";
	command += " --of \"" + config::Directory::getInstance().getTemporaryPath() + "raw/\"";
	command += " --user " + m_User;
	command += " --pass " + m_Password;
	command += " --ip " + m_Ip;

	return command;
}
